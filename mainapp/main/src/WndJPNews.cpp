// WndCef.cpp : 实现文件
//

#include "stdafx.h"
#include "GGTong.h"
#include "WndCef.h"

// CWndJPNews

IMPLEMENT_DYNAMIC(CWndJPNews, CWnd)

CWndJPNews::CWndJPNews()
{
	m_strUrl  = L"";
	m_handler = NULL;
	m_clrCefBk = RGB(35, 34, 40);
}

CWndJPNews::~CWndJPNews()
{
}

BEGIN_MESSAGE_MAP(CWndJPNews, CWnd)
	//ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

// CWndJPNews 消息处理程序

void CWndJPNews::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	CRect rect;
	GetClientRect(&rect);
	dc.FillSolidRect(rect.left, rect.top ,rect.Width(),rect.Height(), RGB(35,34,40));

	CefRefPtr<CefBrowser> browser = m_handler->GetBrowser();
	if(browser)
	{
		CefWindowHandle hwnd = browser->GetHost()->GetWindowHandle();
		if(HWND hWnd = ::GetWindow(hwnd,GW_CHILD))
		{
			::InvalidateRect(hWnd,NULL,FALSE);
		}
	}
}

int CWndJPNews::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_handler = new ClientHandler();

	CRect rect(0,0,854,532);
	GetClientRect(&rect);
	CefWindowInfo window_info;
	window_info.SetAsChild(m_hWnd, rect);
	CefBrowserSettings browser_settings;
	browser_settings.background_color = CefColorSetARGB(255, m_clrCefBk.GetB(), m_clrCefBk.GetG(), m_clrCefBk.GetR());
	CefBrowserHost::CreateBrowser(window_info, m_handler.get(), _W2A(m_strUrl), browser_settings, NULL);

	return 0;
}

void CWndJPNews::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	if (m_handler != NULL)
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
}

BOOL CWndJPNews::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;
}

void CWndJPNews::SetCefBkColor(Color clrBk)
{
	m_clrCefBk = clrBk;
}