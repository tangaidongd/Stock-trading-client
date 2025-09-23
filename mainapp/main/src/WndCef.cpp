// WndCef.cpp : 实现文件
//

#include "stdafx.h"
#include "GGTong.h"
#include "WndCef.h"
#include "UrlParser.h"

// CWndCef

IMPLEMENT_DYNAMIC(CWndCef, CWnd)

CWndCef::CWndCef()
{
	m_handler = NULL;
	m_strUrl = _T("about:blank");
    m_bNeedReloadURL = false;
	m_clrCefBk = RGB(30, 36, 40);
}

CWndCef::~CWndCef()
{
}

BEGIN_MESSAGE_MAP(CWndCef, CWnd)
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_WM_DESTROY()
	ON_COMMAND(UM_CEF_AFTERCREATED, OnCefAfterCreated)
END_MESSAGE_MAP()

void CWndCef::OnPaint()
{
	CPaintDC dc(this);

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

int CWndCef::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_handler = new ClientHandler(m_hWnd);

	CRect rect;
	GetClientRect(&rect);
	CefWindowInfo window_info;
	window_info.SetAsChild(m_hWnd, rect);
	CefBrowserSettings browser_settings;
	browser_settings.background_color = CefColorSetARGB(255, m_clrCefBk.GetB(), m_clrCefBk.GetG(), m_clrCefBk.GetR());
	CefBrowserHost::CreateBrowser(window_info,
								  m_handler.get(), 
								  _W2A(m_strUrl),
								  browser_settings,
								  NULL);

	return 0;
}

void CWndCef::OnDestroy()
{
	CWnd::OnDestroy();
}

void CWndCef::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	if(cx > 0 && cy > 0)
	{
		CefRefPtr<CefBrowser> browser = m_handler->GetBrowser();
		if(browser)
		{
			CefWindowHandle hwnd = browser->GetHost()->GetWindowHandle();
			::MoveWindow(hwnd, 0, 0, cx, cy, TRUE);
		}
	}
}

void CWndCef::OnCefAfterCreated()
{
	CefRefPtr<CefBrowser> browser = m_handler->GetBrowser();
	if(browser)
	{
		CRect rect;
		GetClientRect(&rect);

		CefWindowHandle hwnd = browser->GetHost()->GetWindowHandle();
		::MoveWindow(hwnd, 0, 0, rect.Width(), rect.Height(), TRUE);

        if(m_bNeedReloadURL)
        {
            browser->GetMainFrame()->LoadURL(_W2A(m_strUrl));
            m_bNeedReloadURL = false;
        }
	}
}

BOOL CWndCef::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;
}

BOOL CWndCef::CreateWndCef(UINT nID,CWnd* pParentWnd,const RECT& rect, const CString& strUrl/* = L""*/)
{
	if (!strUrl.IsEmpty())
	{
		SetUrl(strUrl);
	}
	return Create(NULL,NULL,WS_CHILD|WS_VISIBLE,rect, pParentWnd, nID);
}

void CWndCef::SetUrl(const CString& strUrl, const CString& StrParam)
{
	CGGTongApp *pApp  =  (CGGTongApp*) AfxGetApp();
	CGGTongDoc *pDoc = pApp->m_pDocument;
	if (NULL != pDoc->m_pAutherManager)
	{
		UrlParser uParser(strUrl);
		CString strToken = pDoc->m_pAutherManager->GetInterface()->GetServiceDispose()->GetToken();
		CString StrFrom = L"PC";
		uParser.SetQueryValue(L"token", strToken);
		uParser.SetQueryValue(L"regFrom", StrFrom);
		m_strUrl = uParser.GetUrl();
	}
	else
	{
		m_strUrl = strUrl;
	}

	if (!StrParam.IsEmpty())
	{
		m_strUrl += StrParam;
	}
}

void CWndCef::OpenUrl(const CString& strUrl)
{
	SetUrl(strUrl);
	if (NULL != m_handler)
	{
		CefRefPtr<CefBrowser> browser = m_handler->GetBrowser();
		if(browser)
		{
			m_bNeedReloadURL = false;
			browser->GetMainFrame()->LoadURL(_W2A(m_strUrl));
		}
	}
}

void CWndCef::ReLoadCfmUrl(bool32 bActive)
{
	if (NULL == m_handler)
	{
		return;
	}
	CefRefPtr<CefBrowser> browser = m_handler->GetBrowser();

	if (browser)
	{
		if (bActive)
		{
			browser->GetMainFrame()->LoadURL(_W2A(m_strUrl));
		}
		else
		{
			browser->GetMainFrame()->LoadURL("about:blank");
		/*	TCHAR szPath[MAX_PATH] = {0};
			GetModuleFileName(NULL, szPath, MAX_PATH);
			CString StrPath = szPath;

			int pos = StrPath.ReverseFind('\\');
			if (pos > 0)
			{ 
				StrPath = StrPath.Left(pos + 1);
				StrPath = _T("file:///") + StrPath;
				StrPath += _T("html/default.html");

				browser->GetMainFrame()->LoadURL(_W2A(StrPath));
			}*/
		}
	}
}

void CWndCef::ReLoad()
{
	if (NULL != m_handler)
	{
		CefRefPtr<CefBrowser> browser = m_handler->GetBrowser();
		if(browser)
		{
			browser->Reload();
		}
	}
}

void CWndCef::GoBack()
{
	if (NULL != m_handler)
	{
		CefRefPtr<CefBrowser> browser = m_handler->GetBrowser();
		if(browser)
		{
			browser->GoBack();
		}
	}
}

void CWndCef::GoForward()
{
	if (NULL != m_handler)
	{
		CefRefPtr<CefBrowser> browser = m_handler->GetBrowser();
		if(browser)
		{
			browser->GoForward();
		}
	}
}

void CWndCef::SetCefBkColor(Color clrBk)
{
	m_clrCefBk = clrBk;
}

void CWndCef::TransferFunAndRefreshWeb(string strTransData)
{
	if (NULL != m_handler)
	{
		m_handler->SetFocusStatus(false);
		m_handler->SetLoadData(strTransData);
	}
}

void CWndCef::FrameExecuteJavaScript(char* pstrFun)
{
	if (NULL != m_handler)
	{
		CefRefPtr<CefBrowser> browser = m_handler->GetBrowser();
		if (browser)
		{
			CefString strCode(pstrFun); 
			CefString strURL = "about:blank";
			browser->GetMainFrame()->ExecuteJavaScript(strCode, strURL , 0);
		}
	}
}

void CWndCef::SetWebFocusStatus(bool32 bFocusStatus)
{
	if (NULL != m_handler)
	{
		m_handler->SetFocusStatus(bFocusStatus);
	}
}
