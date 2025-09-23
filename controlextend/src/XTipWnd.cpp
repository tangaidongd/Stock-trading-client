// XTipWnd.cpp : implementation file
//

#include "stdafx.h"
#include "XTipWnd.h"
#include "ColorStep.h"
#include "memdc.h"
#include "typedef.h"
#ifdef _DEBUG
#define new DEBUG_NEW 
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define XTIPWND_CLASSNAME _T("XTipWnd")

static const int32 KiTimerIdHide		= 11;
static const int32 KiTimerPeriodHide	= 100;

/////////////////////////////////////////////////////////////////////////////
// CXTipWnd

CXTipWnd::CXTipWnd()
{
	m_FontNormal.CreateFont(-13,0,0,0,400,0,0,0,0,1,2,1,34,L"MS Sans Serif");

	//
	m_pOld = CPoint(0,0);
	CString str=_T("1223345667889");
	
	m_clrTitleBackgroud = RGB(0, 64, 128);
	m_clrContentBackgroud = RGB(226, 223, 222);
	m_clrTitleText = RGB(255, 255, 255);
	m_clrContentText = RGB(0, 0, 0);


	//	fnt.CreatePointFont( 90, "宋体" );
	m_iWidth = 0;
	m_pParentWnd = 0;
	WNDCLASS wndcls;
	HINSTANCE hInst = AfxGetInstanceHandle();
	
	if(!(::GetClassInfo(hInst, XTIPWND_CLASSNAME, &wndcls)))
	{
		// otherwise we need to register a new class
		wndcls.style			= CS_SAVEBITS ;
		wndcls.lpfnWndProc		= ::DefWindowProc;
		wndcls.cbClsExtra		= wndcls.cbWndExtra = 0;
		wndcls.hInstance		= hInst;
		wndcls.hIcon			= NULL;
		wndcls.hCursor			= LoadCursor( hInst, IDC_ARROW );
		wndcls.hbrBackground	= NULL;//(HBRUSH)(COLOR_INFOBK + 1); 
		wndcls.lpszMenuName		= NULL;
		wndcls.lpszClassName	= XTIPWND_CLASSNAME;
		
		if (!AfxRegisterClass(&wndcls))
			AfxThrowResourceException();
	}
	
	m_bWindowsPosition = false;
	m_fontSize = 12;
	m_iHeight = 0;
	m_iLineHeight = 0;
	m_bShowTitle = false;
}

CXTipWnd::~CXTipWnd()
{
	m_FontNormal.DeleteObject();
}


BEGIN_MESSAGE_MAP(CXTipWnd, CWnd)
//{{AFX_MSG_MAP(CXTipWnd)
ON_WM_PAINT()
ON_WM_MOUSEMOVE()
ON_WM_TIMER()
//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CXTipWnd message handlers

int CXTipWnd::CalcSize()
{
	CDC* pDC = GetDC();
	
	pDC->SetTextAlign( TA_LEFT );//TA_CENTER||
	pDC->SetBkMode(TRANSPARENT);	

	m_iLineHeight=(pDC->GetOutputTextExtent(_T("上海"))).cy+2;

	int count=m_stringList.GetCount ();
	m_iHeight=count*m_iLineHeight + 2;
	if (m_bShowTitle)
		m_iHeight += m_iLineHeight;

	CString str;
	CFont* pOldFont=pDC->SelectObject(&m_FontNormal);

	int len;
	m_iWidth=0;
	// xl 2011-01-27 没行的宽度不能超过整个屏幕的1/3
	CRect rectWorkArea;
 	SystemParametersInfo(SPI_GETWORKAREA,0,&rectWorkArea,0);
	const int iMaxWidth = rectWorkArea.Width()*1/3;
    for(POSITION pos = m_stringList.GetHeadPosition(); pos != NULL; )  
    {
		str=(m_stringList.GetNext(pos));
		len=5+(pDC->GetOutputTextExtent(str)).cx;

		len = min(iMaxWidth, len);	// 限制宽度

		if(len>m_iWidth)
			m_iWidth=len;
    }
	
	if (m_bShowTitle)
	{
		len=5+(pDC->GetOutputTextExtent(m_strTitle)).cx;

		len = min(iMaxWidth, len);	// 限制宽度

		if(len>m_iWidth)
			m_iWidth=len;	
	}

    pDC->SelectObject(pOldFont);
	ReleaseDC(pDC);
	return m_iWidth;
}

void CXTipWnd::OnMouseMove(UINT nFlags, CPoint point) 
{
	CWnd::OnMouseMove( nFlags,  point) ;
}

void CXTipWnd::OnPaint() 
{
	CPaintDC dcPaint(this); // device context for painting
	CalcSize();
	// TODO: Add your message handler code here
	CString str;
	
	CRect rt;
	GetClientRect(rt);

	{
		dcPaint.SetBkColor(m_clrContentBackgroud);
		CMemDC dc(&dcPaint, rt);	
		
		// 刷背景
		dc.FillSolidRect(rt, m_clrContentBackgroud);
		
		
		//
		dc.SetBkMode(TRANSPARENT);
		CFont* pOldFont=dc.SelectObject(&m_FontNormal);
		
		// 显示title
		if (m_bShowTitle)
		{
			CRect rectTitle = rt;
			rectTitle.bottom = rectTitle.top + m_iLineHeight;
			
			if ( rectTitle.Height() < 200 )
			{
				CColorStep step;
				step.InitColorRange(10.0f, m_clrTitleBackgroud, rectTitle.Height());
				for ( int32 i=rectTitle.top; i < rectTitle.bottom ; i++ )
				{
					dc.FillSolidRect(rectTitle.left, i, rectTitle.Width(), 1, step.NextColor());
				}
			}
			else
			{
				dc.FillSolidRect(rectTitle, m_clrTitleBackgroud);
			}
			
			dc.SetTextColor(m_clrTitleText);
			dc.DrawText(m_strTitle, rectTitle, DT_CENTER | DT_VCENTER |DT_END_ELLIPSIS /*| DT_SINGLELINE*/);	// 截断过长的数据
		}
		
		// 显示下面的文字
		dc.SetTextColor(m_clrContentText);
		dc.SetTextAlign( TA_LEFT );//TA_CENTER||
		
		int i = 0;
		if (m_bShowTitle) 
		{
			i = 1;
		}			
		
		//lint --e{441} suppress "for clause irregularity: loop variable 'i' not found in 2nd for expression: for循环第2个表达式不存在i，不影响。"
		for(POSITION pos = m_stringList.GetHeadPosition(); pos != NULL;i++ )  
		{
			str=m_stringList.GetNext( pos );   
			//dc.TextOut (2,i*m_iLineHeight+1,str);

			CRect rc(2, i*m_iLineHeight, 2+m_iWidth, (i+1)*m_iLineHeight);
			dc.DrawText(str, rc, DT_LEFT |DT_VCENTER |DT_END_ELLIPSIS);
		}
		
		//
		dc.SelectObject(pOldFont);
	}	
}

void CXTipWnd::SetString(CString &s)
{
	// xl 201111-01-27 现在的字串不能不能超过n行
	const int iMaxRowCount = 10;
	
	m_stringList.RemoveAll ();
	int start=0;
	while(start!=-1)
	{
		int first=start;
		start=s.Find (L"\n",start);
		CString tmp;
		if(start!=-1)
		{
			if ( m_stringList.GetCount() > iMaxRowCount )
			{
				tmp = _T("... ...");		// 超出行数限制
				m_stringList.AddTail(tmp);
				break;
			}
			tmp=s.Mid( first, start-first);
			m_stringList.AddTail (tmp);
			start++;
		}
		else
		{
			// +上最后一行
			if ( s.GetLength() > first )
			{
				tmp = s.Mid(first);
				tmp.TrimLeft(_T("\r\n"));
				if ( !tmp.IsEmpty() )
				{
					m_stringList.AddTail(tmp);
				}
			}
			break;
		}
	}

	if (m_stringList.GetCount() == 0)
	{
		if (s.GetLength() > 0)
			m_stringList.AddTail(s);
	}
}

BOOL CXTipWnd::Create(CWnd * pParentWnd)
{
	ASSERT_VALID(pParentWnd);
	
	DWORD dwStyle = WS_BORDER | WS_POPUP; 
	DWORD dwExStyle = WS_EX_TOOLWINDOW/* | WS_EX_TOPMOST*/;
	m_pParentWnd = pParentWnd;
	
	return CreateEx( dwExStyle, XTIPWND_CLASSNAME, NULL, dwStyle, CRect(0, 0, 0, 0), 
		m_pParentWnd, NULL, NULL );
}

// Show 		 - Show the titletip if needed
// lpszTitleText - The text to be displayed
void CXTipWnd::Show(CPoint point, LPCTSTR lpszContentText, LPCTSTR lpszTitleText, E_ShowTitle eShowTile)
{
	// TRACE(L" %x Show: %s \n", this, (CString)lpszContentText);

	ASSERT( ::IsWindow( GetSafeHwnd() ) );
	
	// If titletip is already displayed, don't do anything.
	//	if( IsWindowVisible() ) 
	//		return;
	
	// Do not display the titletip is app does not have focus
	if( GetFocus() == NULL )
		return;
	
	if (NULL == m_pParentWnd)
	{
		return;
	}

	if ( !::IsWindowVisible(m_pParentWnd->GetSafeHwnd()) )
	{
		return;
	}

	// Determine the width of the text
	if ( !m_bWindowsPosition)
	{
		m_pParentWnd->ClientToScreen( &point );
	}
	
	CClientDC dc(this);
	
	m_strTitle = lpszTitleText;
	if (ESTShow == eShowTile)
		m_bShowTitle = TRUE;
	else if (ESTNotShow == eShowTile)
		m_bShowTitle = FALSE;
	else
	{
		if (m_strTitle.GetLength() > 0)
			m_bShowTitle = TRUE;
		else
			m_bShowTitle = FALSE;
	}

	CString t(lpszContentText);
	SetString (t);
	CalcSize ();
	
	m_pOld = point;

	//
	m_pOld.x += 15;
	m_pOld.y += 15;
	
	// int iMaxX = GetSystemMetrics(SM_CXSCREEN);
	// int iMaxY = GetSystemMetrics(SM_CYSCREEN);

	// 应该得到系统工作区的大小,因为系统有状态栏,而且方向不定.

	CRect rectWorkArea;
 	SystemParametersInfo(SPI_GETWORKAREA,0,&rectWorkArea,0);	
	
	int iMaxX = rectWorkArea.Width();
	int iMaxY = rectWorkArea.Height();	

	if (m_pOld.x + m_iWidth >= iMaxX)
		m_pOld.x -= (m_iWidth + 10);
	if (m_pOld.y + m_iHeight >= iMaxY)
		m_pOld.y -= (2*m_iHeight + 10);
	
	// 显示区域不能覆盖到m_pOld上，否则会有很多的WM_MOUSEMOVE消息由这个窗口转发给Parent
	SetWindowPos(&wndTop,m_pOld.x,m_pOld.y,
		m_iWidth,m_iHeight,SWP_SHOWWINDOW|SWP_NOACTIVATE);//SWP_NOREDRAW);



//	SetWindowPos(&wndTop,m_pOld.x,m_pOld.y,
//		m_iWidth,m_iHeight,SWP_SHOWWINDOW|SWP_NOACTIVATE);//SWP_NOREDRAW);

	//Invalidate();
	RedrawWindow();

//	SetTimer(10,200,NULL);
	SetTimer(KiTimerIdHide, KiTimerPeriodHide, NULL);
}

void CXTipWnd::Hide()
{
	if (!::IsWindow(GetSafeHwnd()))
        return;
	
    if (GetCapture()->GetSafeHwnd() == GetSafeHwnd())
		ReleaseCapture();
	
	ShowWindow( SW_HIDE );

	// TRACE(L" %x Hide \n", this);
}

BOOL CXTipWnd::PreTranslateMessage(MSG* pMsg) 
{
	CWnd *pWnd;
	CWnd *pWndMsg = CWnd::FromHandlePermanent(pMsg->hwnd);
	if ( NULL == pWndMsg )
	{
		return FALSE;
	}
	int hittest;
	switch (pMsg->message)
	{
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_MBUTTONDOWN:
		{
			POINTS pts = MAKEPOINTS( pMsg->lParam );
			POINT  point;
			point.x = pts.x;
			point.y = pts.y;
			pWndMsg->ClientToScreen( &point );
			pWnd = WindowFromPoint( point );
			if( pWnd == this ) 
				pWnd = m_pParentWnd;

			if ( NULL == pWnd )
			{
				return FALSE;
			}

			if ( NULL == pWnd )
			{
				return FALSE;
			}

			hittest = (int)pWnd->SendMessage(WM_NCHITTEST,0,MAKELONG(point.x,point.y));

			if (hittest == HTCLIENT) 
			{
				pWnd->ScreenToClient( &point );
				pMsg->lParam = MAKELONG(point.x,point.y);
			}
			else 
			{
				switch (pMsg->message)
				{
				case WM_LBUTTONDOWN: 
					pMsg->message = WM_NCLBUTTONDOWN;
					break;
				case WM_RBUTTONDOWN: 
					pMsg->message = WM_NCRBUTTONDOWN;
					break;
				case WM_MBUTTONDOWN: 
					pMsg->message = WM_NCMBUTTONDOWN;
					break;
				}
				pMsg->wParam = hittest;
				pMsg->lParam = MAKELONG(point.x,point.y);
			}

			Hide();
			if ( pWndMsg == this )		// 唯有消息是自己窗口出来的才发送
			{
				pWnd->PostMessage(pMsg->message,pMsg->wParam,pMsg->lParam);
			}
			return TRUE;
		}	
		
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
        Hide();
		if ( pWndMsg == this )
		{
			m_pParentWnd->PostMessage( pMsg->message, pMsg->wParam, pMsg->lParam );
		}
		return TRUE;
	case WM_MOUSELEAVE:
		if ( pWndMsg == m_pParentWnd )
		{
			Hide();
			return TRUE;
		}
		break;
	case WM_MOUSEMOVE:
		if ( pWndMsg == m_pParentWnd )
		{
			TRACKMOUSEEVENT tme;
			tme.cbSize = sizeof(tme);
			tme.dwFlags = TME_LEAVE;
			tme.hwndTrack = pMsg->hwnd;
			_TrackMouseEvent(&tme);
			return TRUE;
		}
		break;
	}
	
	if( GetFocus() == NULL )
	{
        Hide();
		return TRUE;
	}
	
	return CWnd::PreTranslateMessage(pMsg);
}

void CXTipWnd::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	if ( 10 == nIDEvent )
	{
		KillTimer(10);
		SetWindowPos(&wndTop,m_pOld.x,m_pOld.y,
		m_iWidth,m_iHeight,SWP_SHOWWINDOW|SWP_NOACTIVATE);//SWP_NOREDRAW);
	}
	else if ( KiTimerIdHide == nIDEvent )
	{
		if (NULL != m_pParentWnd)
		{
			if ( !::IsWindowVisible(m_pParentWnd->GetSafeHwnd()) && IsWindowVisible())
			{
				Hide();
				KillTimer(KiTimerIdHide);
			}
		}
		
	}

	CWnd::OnTimer(nIDEvent);
}


