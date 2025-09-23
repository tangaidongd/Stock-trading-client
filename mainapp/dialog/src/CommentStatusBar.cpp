// StatusBarEx.cpp : implementation file
//

#include "stdafx.h"
#include "dlgspirite.h"
#include "dlgnetwork.h"
#include "DlgRecentMerch.h"
#include "dlgindexprompt.h"
#include "DlgAlarm.h"
#include "memdc.h"
//#include "viewdata.h"
#include "IoViewReport.h"
#include "IoViewManager.h"
#include "GdiPlusTS.h"
#include "FontFactory.h"
#include "UtilTool.h"
//#include "communication.h"
#include "NewsEngineManager.h"
#include "CommentStatusBar.h"
#include "BalloonMsg.h"
#include "XmlShare.h"
#include "afxconv.h"
#include "PathFactory.h"
#include "DlgInputShowName.h"
#include "sha1.h"
#include "Proxy_auth_client_base.h"
#include "CCodeFile.h"
#include "XmlShare.h"
#include "coding.h"
#include "ConfigInfo.h"
#include "DlgNews.h"
#include "DlgTodayComment.h"
#include "facescheme.h"
#include "UrlParser.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifndef IDC_HAND
#define IDC_HAND MAKEINTRESOURCE(32649)		// From WINUSER.H
#endif

/////////////////////////////////////////////////////////////////////////////
// CCommentStatusBar
#define  GOLDCOMMENTCOUNT 100

const int32 COMMENT_SEPARETE = 50;

// 
const int32 KTimeIdHorseRunId				= 452140;		// 定时刷新
const int32 KTimePeriodHorseRun			    = 55;			// 每100毫秒刷一次

const int32 KTimeRerequestCommentId         = 452141;       //重新请求新闻
const int32 KTimePeriodRerequestComment     = 5000;         //5秒


CCommentStatusBar::CCommentStatusBar()
{
	// 背景图(用主标题栏一样的背景)
	m_pImageBkGround = NULL;
	
	if ( !ImageFromIDResource(AfxGetResourceHandle(),IDR_PNG_COMMSTATUSBAR, L"PNG", m_pImageBkGround))
	{
		//ASSERT(0);
		m_pImageBkGround = NULL;
	}

	m_ptLastMouseMove = CPoint(-1, -1);

	m_bMouseTrack = FALSE;

	/* test
	T_OrgGOLDCOMMENT adv;
	adv.m_StrTitle = L"成功跑马！";
	adv.m_StrContent = L"1号马子";
	adv.m_iIndex = 0;
	m_aOrgAdvs.Add(adv);

	adv.m_StrContent = L"2号马子";
	adv.m_iIndex = 1;
	m_aOrgAdvs.Add(adv);
	
	
	adv.m_StrContent = L"3号马子";
	adv.m_iIndex = 2;
	m_aOrgAdvs.Add(adv);
	*/
	m_bRun = FALSE;
	m_bAddItemComment = false;
	m_iIndexID = -1;
	m_iSelIndexID = 0;
	m_dwGetDataTickComment = 0;

	m_clrNorText = RGB(198, 177, 31);
	m_clrSelText = RGB(238,69,2);
	m_clrOtherText = RGB(255,255,255);

	CGGTongApp *pApp = (CGGTongApp *)AfxGetApp();
	CGGTongDoc *pDoc = pApp->m_pDocument;
	CAbsCenterManager *pAbsCenterManager = pDoc->m_pAbsCenterManager;
	m_iInfoType = pAbsCenterManager->GetJinpingID();

	m_bShowJp = TRUE;
	m_bShowInfo = FALSE;
	m_handler = NULL;
}

CCommentStatusBar::~CCommentStatusBar()
{
	DEL(m_pImageBkGround);
}

BEGIN_MESSAGE_MAP(CCommentStatusBar, CStatusBar)
	//{{AFX_MSG_MAP(CCommentStatusBar)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_COMMAND(UM_CEF_AFTERCREATED, OnCefAfterCreated)
	//ON_WM_LBUTTONDOWN()
	//ON_WM_RBUTTONDOWN()
	//ON_WM_LBUTTONDBLCLK()
	//ON_WM_TIMER()
	//ON_WM_MOUSEMOVE()
	//ON_MESSAGE(WM_MOUSEHOVER, OnMouseHover)
	//ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
	//ON_WM_DESTROY()
	ON_WM_SETCURSOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCommentStatusBar message handlers

BOOL CCommentStatusBar::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;
}

int CCommentStatusBar::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CStatusBar::OnCreate(lpCreateStruct) == -1)
		return -1;

	CStatusBar::GetStatusBarCtrl().SetMinHeight(41);
	
	UrlParser uParser(L"alias://webnewsscroll");

	CString strValue = uParser.GetQuery();
	strValue = CIoViewNews::GetOrgCode(strValue);
	uParser.SetQuery(strValue);

	m_strUrl = uParser.GetUrl();

	return 0;
}

void CCommentStatusBar::OnPaint() 
{
	CPaintDC pdc(this); 

	//CRect Rect;
	//GetClientRect(&Rect);

	//CMemDC dc(&pdc, Rect);
	//int32 iOldMode = dc.SetBkMode(TRANSPARENT);
	//
	//CPen pen,*pOldPen;
 //	pen.CreatePen(PS_SOLID,1,CFaceScheme::Instance()->GetSysColor(ESCText));
 //	pOldPen = (CPen*)dc.SelectObject(&pen);
	//
	//// 字色,字体	
	//dc.SetTextColor( RGB(0,0,0) );
	//LOGFONT FontStatus;
	//InitStatusBBSLogFont(FontStatus);
	//
	//CFont FontSts;
	//
	//BOOL bOK = FontSts.CreateFontIndirect(&FontStatus);
	//if (!bOK)
	//{
	//	return;
	//}
	//
	//CFont* pOldFont = dc.SelectObject(&FontSts);
	////
	//
	//COLORREF clrActive = RGB(251, 206, 3);//CFaceScheme::Instance()->GetActiveColor();
	//COLORREF clrBefore = dc.SetTextColor(clrActive);
	//
	//dc.SetTextColor(clrBefore);

	//DrawRectBBS(&dc,m_RectBBS, m_RectShow);

	//dc.SelectObject(pOldPen);
	//dc.SetBkMode(iOldMode);
	//dc.SelectObject(pOldFont);
	//
	//pen.DeleteObject();
	//FontSts.DeleteObject();
}

void CCommentStatusBar::InitStatusBBSLogFont( OUT LOGFONT &lf )
{
	memset(&lf, 0, sizeof(lf));
	
	_tcscpy(lf.lfFaceName , gFontFactory.GetExistFontName(L"微软雅黑"));	//...
   	lf.lfHeight  = -14;
   	lf.lfWeight  = 400;
   	lf.lfCharSet = 0;
	lf.lfOutPrecision = 3;
}

void CCommentStatusBar::OnCefAfterCreated()
{
	CefRefPtr<CefBrowser> browser = m_handler->GetBrowser();
	if(browser)
	{
		CRect rect;
		GetClientRect(&rect);

		CefWindowHandle hwnd = browser->GetHost()->GetWindowHandle();
		::MoveWindow(hwnd, 0, 0, rect.Width(), rect.Height(), TRUE);
	}
}

void CCommentStatusBar::OnSize(UINT nType, int cx, int cy) 
{
	if(cx > 0 && cy > 0)
	{
		if(m_handler == NULL)
		{
			m_handler = new ClientHandler(m_hWnd);

			CRect rect;
			GetClientRect(&rect);
			CefWindowInfo window_info;
			window_info.SetAsChild(m_hWnd, rect);
			CefBrowserSettings browser_settings;
			CefBrowserHost::CreateBrowser(window_info,m_handler.get(),_W2A(m_strUrl),browser_settings,NULL);
		}
		else
		{
			CefRefPtr<CefBrowser> browser = m_handler->GetBrowser();
			if(browser)
			{
				CRect rect;
				GetParent()->GetClientRect(&rect);
				CefWindowHandle hwnd = browser->GetHost()->GetWindowHandle();
				::MoveWindow(hwnd, 0, 0, rect.Width(), rect.Height(), TRUE);
			}
		}
	}

	CStatusBar::OnSize(nType,cx,cy);
}

CSize CCommentStatusBar::GetProperBBSSizeShow()
{
	CSize SizeShow(0, 0);
	CClientDC dc(this);
	LOGFONT FontStatus;
	InitStatusBBSLogFont(FontStatus);
	CFont FontSts;
	FontSts.CreateFontIndirect(&FontStatus);
	CFont *pOldFont = dc.SelectObject(&FontSts);
	const int32 iSeparate = COMMENT_SEPARETE;
	
	int32 i = 0;
	for ( i=0; i < m_aOrgAdvs.GetSize() ; i++ )
	{
		CSize SizeText = dc.GetTextExtent(m_aOrgAdvs[i].m_StrContent);
		SizeShow.cx += SizeText.cx;
		SizeShow.cx += iSeparate;
	}
	dc.SelectObject(pOldFont);
	
	if ( i == 0 )
	{
		// 没有公告, holyshit
		return m_RectBBS.Width();	// 返回默认的
	}
	else
	{
		SizeShow.cx -= iSeparate;	// 剔除最后的一个分割
	}
	
	return SizeShow;
}

void CCommentStatusBar::DrawRectBBS(CDC* pDC, CRect& rt, CRect &RectShow)
{
	// 填充背景色:
	Graphics GraphicImage(pDC->GetSafeHdc());
	CRect rectTest = rt;
	rectTest.right+= 10;
	
	if (NULL != m_pImageBkGround)
	{
		DrawImage(GraphicImage, m_pImageBkGround, rectTest, 1, 0, true);
	}
		
	CRect RectDraw(rt);
	RectDraw.left += 5;
	CRgn rgnOld, rgnNew;
	rgnOld.CreateRectRgn(0,0,0,0);
	rgnNew.CreateRectRgnIndirect(RectDraw);
	BOOL bRgnOld = GetClipRgn(pDC->GetSafeHdc(), (HRGN)rgnOld.m_hObject) > 0;
	pDC->SelectClipRgn(&rgnNew);

	if( RectShow.right > rt.left)
	{
		// 根据当前广告的数量，从左到右绘制
		// rt为BBS的rect， RectShow为当前显示最左侧位置
		if ( m_aOrgAdvs.GetSize() <= 0 )
		{
			pDC->SetTextColor(m_clrNorText);
			// 默认绘制
			CRect rect = RectShow;
			rect.right = rt.right;
			rect.right -= 5;
			rect.top += 1;
			CString StrText;
			// StrText.Format(_T("欢迎使用%s"), AfxGetApp()->m_pszAppName);
			pDC->DrawText(StrText, &rect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
			CSize SizeText = pDC->GetTextExtent(StrText);

			RectShow.left -= 1;
			RectShow.right -= 1;

			if ( RectShow.left+SizeText.cx < RectDraw.left )
			{
				const int32 iWidth = RectShow.Width();
				RectShow.left = rt.right - 10;
				RectShow.right = RectShow.left + iWidth;
			}
		}
		else
		{
			// 广告, rt可见区域 RectShow总区域
			CRect rect = RectShow;
			rect.right = rt.right;	// 绘制区域拓展
			rect.right -= 5;
			rect.top += 1;

			const int32 iSeparate = COMMENT_SEPARETE;
			const int32 iAdvCount = m_aOrgAdvs.GetSize();
			m_aOrgAdvRects.SetSize(0, iAdvCount);

			// 这种为一轮一轮的
			for ( int32 i=0; i < iAdvCount ; i++ )
			{
				if (KiIndexNormal == m_aOrgAdvs[i].m_iInfoIndex)
				{
					pDC->SetTextColor(m_clrOtherText);
				}
				else
				{
					pDC->SetTextColor(m_clrNorText);
				}

				CSize SizeText = pDC->GetTextExtent(m_aOrgAdvs[i].m_StrContent);
				const int32 iWidth = SizeText.cx + iSeparate;
				if ( rect.left + iWidth > rt.left )
				{
					// 于可见范围内
					pDC->DrawText(m_aOrgAdvs[i].m_StrContent, &rect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
					T_OrgCommRect AdvRect;
					AdvRect.SetRect(max(rect.left, RectDraw.left), rect.top, min(rect.left+SizeText.cx, rect.right), rect.bottom);
					AdvRect.iOrgIndex = i;
					m_aOrgAdvRects.Add( AdvRect );
				}
				
				rect.left += iWidth;
				
				if ( rect.left >= rect.right )
				{
					break;
				}
			}

			if ( GetAdvIndexByPoint(m_ptLastMouseMove) >= 0 )	// 是否鼠标下
			{
				// 鼠标移动进来则不滚动
			}
			else
			{
				if ( m_bRun )
				{
					RectShow.left -= 1;		
					RectShow.right -= 1;
				}

				m_bRun = FALSE;
				// 重定位
				if ( RectShow.right <= rt.left)
				{
					const int32 iWidth = RectShow.Width();
					RectShow.left = rt.right - 10;
					RectShow.right = RectShow.left + iWidth;
				}
			}
		}
	}

	if ( bRgnOld )
	{
		pDC->SelectClipRgn(&rgnOld);
	}
	else
	{
		pDC->SelectClipRgn(NULL);
	}
}

int32 CCommentStatusBar::GetAdvIndexByPoint( CPoint pt )
{
	for ( int32 i=0; i < m_aOrgAdvRects.GetSize() ; i++ )
	{
		if ( m_aOrgAdvRects[i].PtInRect(pt) )
		{
			return m_aOrgAdvRects[i].iOrgIndex;
		}
	}
	return -1;
}

void CCommentStatusBar::OnMouseMove(UINT nFlags, CPoint point) 
{
	if ( !m_bMouseTrack )
	{
		TRACKMOUSEEVENT tme = {0};
		tme.cbSize  = sizeof(tme);
		tme.dwFlags = TME_HOVER | TME_LEAVE; // 停留标志 与 离开标志
		tme.dwHoverTime = 400; // 400 ms - 调用Track会重置Hover计时
		tme.hwndTrack = m_hWnd;
		m_bMouseTrack = _TrackMouseEvent(&tme);
	}
	
	// 检查是否悬浮在广告上面
	bool32 bInAdv = GetAdvIndexByPoint(point) >= 0;
	m_ptLastMouseMove = point;
	
	SetCursor(LoadCursor(NULL, bInAdv ? MAKEINTRESOURCE(32649)/*IDC_HAND*/ : IDC_ARROW));
	
	CStatusBar::OnMouseMove(nFlags, point);
	
	return;
}

LRESULT CCommentStatusBar::OnMouseHover( WPARAM nFlags, LPARAM point )
{
	m_bMouseTrack = FALSE;
	return ::DefWindowProc(m_hWnd, WM_MOUSEHOVER, nFlags, point);
}

LRESULT CCommentStatusBar::OnMouseLeave(WPARAM w, LPARAM l)
{
	m_bMouseTrack = FALSE;
	m_ptLastMouseMove = CPoint(-1, -1);
	
	return ::DefWindowProc(m_hWnd, WM_MOUSELEAVE, w, l);
}

void CCommentStatusBar::SetStatusTimer()
{
	//SetTimer(KTimeRerequestCommentId,KTimePeriodRerequestComment, NULL);
	//SetTimer(KTimeIdHorseRunId, KTimePeriodHorseRun, NULL);
}

void CCommentStatusBar::OnTimer(UINT nIDEvent)
{
	if ( KTimeIdHorseRunId == nIDEvent)
	{
		m_bRun = TRUE;
		Invalidate();
	}

	if ( KTimeRerequestCommentId == nIDEvent ) //没有收到数据，发送请求
	{
		if ( m_bAddItemComment && m_aOrgAdvs.GetSize() > 0 )
		{
			KillTimer(nIDEvent);
		}
		else
		{
			if (m_bShowJp)
			{
				CGGTongApp *pApp = (CGGTongApp *)AfxGetApp();
				CGGTongDoc *pDoc = pApp->m_pDocument;
				CAbsCenterManager *pAbsCenterManager = pDoc->m_pAbsCenterManager;
				m_iInfoType = pAbsCenterManager->GetJinpingID();
				RequesComments(m_iInfoType); 
			}
			if (m_bShowInfo)
			{
				RequesComments(KiIndexNormal); 
			}
		}
	}
}


void CCommentStatusBar::HitTestRectData(CPoint pt, int32 iType)
{	
	// 是否在金评上面
	for ( int i=0; i < m_aOrgAdvRects.GetSize() ; i++ )
	{
		if ( m_aOrgAdvRects[i].PtInRect(pt) )
		{
			// 打开该广告
			const int32 iIndex = m_aOrgAdvRects[i].iOrgIndex;
			ASSERT( iIndex >= 0 && iIndex < m_aOrgAdvs.GetSize() );
			if ( iIndex >= 0 && iIndex < m_aOrgAdvs.GetSize() )
			{
				m_iIndexID = m_aOrgAdvs[iIndex].m_iIndexID;

				if ( m_iIndexID > 0)
				{
					m_iSelIndexID = iIndex;
					RequestCommentContent(m_iIndexID);
				}
			}
			return;
		}
	}
}

void CCommentStatusBar::OnLButtonDown(UINT nFlags, CPoint point) 
{
	HitTestRectData ( point, 0 );	
	CStatusBar::OnLButtonDown(nFlags, point);
}

void CCommentStatusBar::OnRButtonDown(UINT nFlags, CPoint point) 
{
	CGGTongApp *pApp = (CGGTongApp *)AfxGetApp();
	CNewMenu menu;
	menu.CreatePopupMenu();

	menu.AppendMenu(MF_STRING, 1, pApp->m_strNewsInfo1);
	menu.AppendMenu(MF_STRING, 2, pApp->m_strNewsInfo2);
//	menu.AppendMenu(MF_STRING, 3, pApp->m_strNewsInfo3);

	if (m_bShowJp)
	{
		menu.CheckMenuItem(1, MF_BYCOMMAND | MF_CHECKED );
	}
	else
	{
		menu.CheckMenuItem(1, MF_BYCOMMAND | MF_UNCHECKED );
	}

	if (m_bShowInfo)
	{
		menu.CheckMenuItem(2, MF_BYCOMMAND | MF_CHECKED );
	}
	else
	{
		menu.CheckMenuItem(2, MF_BYCOMMAND | MF_UNCHECKED );
	}

	CPoint ptMenu(point);
	ClientToScreen(&ptMenu);
	
	CMainFrame *pMainFrame = (CMainFrame *)AfxGetMainWnd();
	ASSERT( NULL != pMainFrame );
	
	int iRet = menu.TrackPopupMenu(TPM_BOTTOMALIGN |TPM_LEFTALIGN |TPM_NONOTIFY |TPM_RETURNCMD, ptMenu.x, ptMenu.y, pMainFrame);
	switch (iRet)
	{
	case 1:	// 金评
		{
			CGGTongDoc *pDoc = pApp->m_pDocument;
			CAbsCenterManager *pAbsCenterManager = pDoc->m_pAbsCenterManager;
			m_iInfoType = pAbsCenterManager->GetJinpingID();
			m_bShowJp = !m_bShowJp;
			m_bAddItemComment = FALSE;
			if (m_bShowJp)
			{
				RequesComments(m_iInfoType);
			}
			else if (!m_bShowJp && !m_bShowInfo)
			{
				m_bShowJp = TRUE;
				RequesComments(m_iInfoType);
			}

			if (!m_bShowJp)
			{
				m_aJP.RemoveAll();
				m_aOrgAdvs.RemoveAll();
				for(int i =0 ; i < m_aInfo.GetSize(); i++)
				{
					T_NewsInfo Temp = m_aInfo.GetAt(i);
					m_aOrgAdvs.Add(Temp);
				}

				m_aOrgAdvRects.RemoveAll();
				CSize SizeShow = GetProperBBSSizeShow();
				m_RectShow = m_RectBBS;
				m_RectShow.OffsetRect(m_RectShow.Width()-10, 0);	// 从最右边开始
				m_RectShow.right = SizeShow.cx + m_RectShow.left;	// 最大宽度
			}
			
		}				
		break;
	case 2:	// 精品资讯
		{
			m_iInfoType = KiIndexNormal;
			m_bShowInfo = !m_bShowInfo;
			m_bAddItemComment = FALSE;
			if (m_bShowInfo)
			{
				RequesComments(m_iInfoType);
			}
			else if(!m_bShowJp && !m_bShowInfo)
			{
				m_bShowInfo = TRUE;
				RequesComments(m_iInfoType);
			}

			if (!m_bShowInfo)
			{
				m_aInfo.RemoveAll();
				m_aOrgAdvs.RemoveAll();
				for( int i =0 ; i < m_aJP.GetSize(); i++)
				{
					T_NewsInfo Temp = m_aJP.GetAt(i);
					m_aOrgAdvs.Add(Temp);
				}
				
				m_aOrgAdvRects.RemoveAll();
				CSize SizeShow = GetProperBBSSizeShow();
				m_RectShow = m_RectBBS;
				m_RectShow.OffsetRect(m_RectShow.Width()-10, 0);	// 从最右边开始
 				m_RectShow.right = SizeShow.cx + m_RectShow.left;	// 最大宽度
			}
			
		}
		break;
	case 3:	// 财经日历
		{
			m_iInfoType = KiIndexNormal;
			if (m_bShowInfo)
			{
				RequesComments(m_iInfoType);
			}
		}
		break;
	default:
		;
	}
	CStatusBar::OnRButtonDown(nFlags, point);
}

//添加当天金评
void CCommentStatusBar::InsertAtComment(T_NewsInfo &stFirst)
{
	if ( stFirst.m_StrContent.GetLength() <= 0 )
	{
		return;
	}
	
	bool32 bAdd = FALSE;
	if (KiIndexNormal == stFirst.m_iInfoIndex)
	{
		if (m_bShowInfo)
		{
			bAdd = TRUE;
		}
	}
	else
	{
		if (m_bShowJp)
		{
			bAdd = TRUE;
		}
	}

	if (bAdd)
	{
		m_aOrgAdvs.InsertAt(0, stFirst,1);
	}
	else
	{
		return;
	}

	//
	CSize SizeShow = GetProperBBSSizeShow();
	m_RectShow = m_RectBBS;
	m_RectShow.OffsetRect(m_RectShow.Width()-10, 0); // 从最右边开始
	m_RectShow.right = SizeShow.cx + m_RectShow.left;		// 最大宽度

	m_aOrgAdvRects.RemoveAll();
}

//
void CCommentStatusBar::AddBatchComment(IN listNewsInfo& listTitles)
{
	if( listTitles.GetSize() <= 0 )
	{
		return;
	}
	
	CString strCondition = L"";
	bool    bfind  = false;

	if (KiIndexNormal == listTitles.GetAt(0).m_iInfoIndex)
	{
		int32 iSize = m_aInfo.GetSize();
		if( !m_bAddItemComment || 0==iSize)
		{
			DWORD dwTick = ::GetTickCount();
			if( m_dwGetDataTickComment!= 0 && ( dwTick - m_dwGetDataTickComment ) < 30*1000  && iSize >0 )
			{
				return ;
			}
			if (!m_bShowInfo)
			{
				return ;
			}
			
			bfind = TRUE;
			m_dwGetDataTickComment = dwTick;
			
			m_aInfo.RemoveAll();
	//		m_aOrgAdvRects.RemoveAll();
			
			for( int i =0 ; i < listTitles.GetSize(); i++)
			{
				T_NewsInfo Temp = listTitles.GetAt(i);
				m_aInfo.Add(Temp);
			}
			
			m_bAddItemComment = true;
			
// 			CSize SizeShow = GetProperBBSSizeShow();
// 			m_RectShow = m_RectBBS;
// 			m_RectShow.OffsetRect(m_RectShow.Width()-10, 0); // 从最右边开始
// 			m_RectShow.right = SizeShow.cx + m_RectShow.left;		// 最大宽度
		}
	}
	else
	{
		int32 iSize = m_aJP.GetSize();
		if( !m_bAddItemComment || 0==iSize)
		{
			DWORD dwTick = ::GetTickCount();
			if( m_dwGetDataTickComment!= 0 && ( dwTick - m_dwGetDataTickComment ) < 30*1000  && iSize >0 )
			{
				return ;
			}
			if (!m_bShowJp)
			{
				return ;
			}
			
			bfind = TRUE;
			m_dwGetDataTickComment = dwTick;
			
			m_aJP.RemoveAll();
	//		m_aOrgAdvRects.RemoveAll();
			//今评
			
			for( int i =0 ; i < listTitles.GetSize(); i++)
			{
				
				T_NewsInfo Temp = listTitles.GetAt(i);
				m_aJP.Add(Temp);
			}
			
			m_bAddItemComment = true;
// 			
// 			CSize SizeShow = GetProperBBSSizeShow();
// 			m_RectShow = m_RectBBS;
// 			m_RectShow.OffsetRect(m_RectShow.Width()-10, 0); // 从最右边开始
// 			m_RectShow.right = SizeShow.cx + m_RectShow.left;		// 最大宽度
		}
	}

	if (bfind)
	{
		m_aOrgAdvs.RemoveAll();
		int i = 0;
		for( i =0 ; i < m_aJP.GetSize(); i++)
		{
			T_NewsInfo Temp = m_aJP.GetAt(i);
			m_aOrgAdvs.Add(Temp);
		}
		
		for(i =0 ; i < m_aInfo.GetSize(); i++)
		{
			T_NewsInfo Temp = m_aInfo.GetAt(i);
			m_aOrgAdvs.Add(Temp);
		}
		
		m_aOrgAdvRects.RemoveAll();
		CSize SizeShow = GetProperBBSSizeShow();
		m_RectShow = m_RectBBS;
		m_RectShow.OffsetRect(m_RectShow.Width()-10, 0);	// 从最右边开始
	 	m_RectShow.right = SizeShow.cx + m_RectShow.left;	// 最大宽度
	}
}

void CCommentStatusBar::OnVDataNewsContentUpdate(const T_NewsInfo& stNewsInfo)
{
	if (stNewsInfo.m_StrContent.GetLength() <= 0)
	{
		CString StrErr = L"抱歉，您访问的页面已经失效。";
		AfxMessageBox(StrErr);
		return;
	}
	
	if( m_iIndexID != stNewsInfo.m_iIndexID)
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
		if (NULL == pMain->m_pDlgJPNews || NULL == pMain->m_pDlgJPNews->m_hWnd)
		{
			pMain->m_pDlgJPNews  = new CDlgJPNews;
			pMain->m_pDlgJPNews ->Create(IDD_DIALOG_JP_NEWS, this);
			pMain->m_pDlgJPNews ->CenterWindow();
		}
		
		CGuiTabWnd	*pGuiTopTabWnd = pMain->m_pDlgJPNews->GetTabWnd();
		CString strCaption = m_aOrgAdvs[m_iSelIndexID].m_StrContent;//stNewsInfo.m_StrContent;
		CString strShort = strCaption.Left(7) + L"...";
		int iTabCnt = pMain->m_pDlgJPNews ->GetTabCount();
		
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
		
		m_iIndexID = -1;
	}
	else
	{
		CDlgNews dlg;
		dlg.m_clrText = GetSysColor(ESCText);
		dlg.m_clrBackground = GetSysColor(ESCBackground);
		dlg.SetTitle( m_aOrgAdvs[m_iSelIndexID].m_StrContent);
		dlg.SetNewsText(stNewsInfo.m_StrContent);
		
		CString strShowText = stNewsInfo.m_StrTimeUpdate;
		CTime timeTemp;
		CUtilTool::GetInistance().CharToTime(strShowText,timeTemp);
		strShowText = timeTemp.Format(L"%m/%d/%Y %H:%M");
		dlg.SetstrTime(strShowText);
		
		dlg.DoModal();
		m_iIndexID = -1;
	}
}

void CCommentStatusBar::RequestCommentContent(int iIndexId)
{
	CGGTongApp *pApp = (CGGTongApp *)AfxGetApp();
	CGGTongDoc *pDocument = pApp->m_pDocument;	
	ASSERT(NULL!= pDocument);
	CAbsCenterManager *pAbsCenterManager = pDocument->m_pAbsCenterManager;
	if ( NULL == pAbsCenterManager )
	{
		return;
	}
 
	CMmiNewsReqNewsContent NewsReqNewsContent ;
	NewsReqNewsContent.m_iIndexID = iIndexId;
	pAbsCenterManager->RequestNews(&NewsReqNewsContent);
}

void CCommentStatusBar::RequesComments(int32 iType)
{
	// 请求金评的新闻标题
	CGGTongApp *pApp = (CGGTongApp *)AfxGetApp();
	CGGTongDoc *pDoc = pApp->m_pDocument;
	CAbsCenterManager *pAbsCenterManager = pDoc->m_pAbsCenterManager;

	CMmiNewsReqTitleList NewsReqJinpingTitle;
	NewsReqJinpingTitle.m_iInfoIndex = iType;
	NewsReqJinpingTitle.m_iBeginPos = 0;
	NewsReqJinpingTitle.m_iNeedCounts = GOLDCOMMENTCOUNT;
	
	pAbsCenterManager->RequestNews(&NewsReqJinpingTitle);
}

