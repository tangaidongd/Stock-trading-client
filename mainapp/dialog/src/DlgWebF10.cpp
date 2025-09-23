// DlgWebF10.cpp : 实现文件
//

#include "stdafx.h"
#include "DlgWebF10.h"
#include "UrlParser.h"
#include "sha1.h"
#include "coding.h"

// CDlgWebF10 对话框

IMPLEMENT_DYNAMIC(CDlgWebF10, CDialog)

CDlgWebF10::CDlgWebF10(CIoViewBase* pIoViewActive, CMerch* pMerch, CWnd* pParent):CDialogEx(CDlgWebF10::IDD, pParent)
{
	CGGTongDoc* pDoc = (CGGTongDoc*)AfxGetDocument();

	m_pAbsCenterManager = pDoc->m_pAbsCenterManager;
	if ( NULL == m_pAbsCenterManager )
	{
		//ASSERT(0);
	}

	//m_pAbsCenterManager->m_pDlgF10 = this;
	m_pIoViewActive	= pIoViewActive;
	m_pMerch		= pMerch;

	m_bMaxed = false;
	m_bInit = false;
}

CDlgWebF10::~CDlgWebF10()
{
	m_strType = _T("001");
}

void CDlgWebF10::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX); 

	DDX_Control(pDX, IDC_RICHEDIT, m_RichEdit);
}

BEGIN_MESSAGE_MAP(CDlgWebF10, CDialogEx)
	ON_WM_CLOSE()
	ON_WM_PAINT()
    ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

#define WEB_ID		WM_USER + 12345

// CDlgWebF10 消息处理程序
BOOL CDlgWebF10::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	
	CRect rcShow;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &rcShow, SPIF_SENDCHANGE);	// 不包括任务栏在内
	int screenWidth = rcShow.Width();
	rcShow.left = screenWidth * 3 / 16;
	rcShow.right = rcShow.left + screenWidth * 5 / 8;
	rcShow.top = 103;
	rcShow.bottom -= 60;
	MoveWindow(rcShow);

	m_RichEdit.ShowWindow(SW_HIDE);
	GetClientRect(rcShow);

	m_bInit = true;
	NavigateWebPage();

	// 展示一个浏览器页面
	m_wndCef.Create(NULL,NULL, WS_CHILD|WS_VISIBLE, rcShow, this, WEB_ID);
	m_wndCef.MoveWindow(rcShow);

	return TRUE;
}

BOOL CDlgWebF10::PreTranslateMessage(MSG* pMsg)
{
	if ( WM_KEYDOWN == pMsg->message )
	{
		if ( VK_ESCAPE == pMsg->wParam || VK_RETURN == pMsg->wParam )
		{
			PostMessage(WM_CLOSE,0,0);
			return TRUE;
		}
		else
		{
			return TRUE;
		}
	}
	else if ( WM_SYSKEYDOWN == pMsg->message )
	{
		if ( VK_F10 == pMsg->wParam )
		{
		
			if (!OnTypeChange(L"002"))
			{
				PostMessage(WM_CLOSE,0,0);
			}
			return TRUE;
		}
		else if ( VK_F11 == pMsg->wParam )
		{

			if (!OnTypeChange(L"001"))
			{
				PostMessage(WM_CLOSE,0,0);
			}
			return TRUE;
		}
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}

void CDlgWebF10::AdjustToMaxSize()
{
	if ( !m_bMaxed )
	{
		return;
	}

	CRect RectClient;

	//
	CMainFrame * pMainFrame = (CMainFrame *)AfxGetMainWnd();	

	::GetWindowRect(pMainFrame->m_hWndMDIClient, &RectClient);

	MoveWindow(&RectClient);
}
void CDlgWebF10::OnClose()
{
	//CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
	//delete pMainFrame->m_pDlgF10;
	//pMainFrame->m_pDlgF10 = NULL;
	
    //m_wndCef.DestroyWindow();
	//DestroyWindow();
	//delete m_pAbsCenterManager->m_pDlgF10;
	//m_pAbsCenterManager->m_pDlgF10 = NULL;
    ShowWindow(SW_HIDE);
}

void CDlgWebF10::OnMerchChange(CMerch* pMerch)
{
	if (pMerch == NULL)
	{
		return;
	}
	
	m_pMerch = pMerch;
	NavigateWebPage();
}

bool CDlgWebF10::OnTypeChange(CString strType,bool Refresh /*= true*/)
{
	if (m_strType == strType)
	{
		return false;
	}
	m_strType = strType;

	if (Refresh)
	{
		NavigateWebPage();
	}
	
	return true;
}

void CDlgWebF10::NavigateWebPage()
{
	if (!m_bInit)
	{
		return;
	}
	
	CGGTongApp *pApp = DYNAMIC_DOWNCAST(CGGTongApp, AfxGetApp());
	if (!pApp)
	{
		return;
	}

	CString StrUrl = L"";

// 	CString strURL =  CIoViewBase::GetServerNewsAddress();
// 	UrlParser urlparser(strURL);

	UrlParser urlparser;
	urlparser.Parser(L"alias://stockF10");
	if (m_strType == _T("002"))
	{
// 		urlparser.SetPath(L"/F10");
		urlparser.SetPath(L"/show/f10view");
		SetWindowText(_T("F10 资料"));
	}
	else
	{
		CString strTitle;
		strTitle.Format(_T("%s的关联资讯"), m_pMerch->m_MerchInfo.m_StrMerchCnName.GetBuffer());
		SetWindowText(strTitle);
//		urlparser.SetPath(L"/F11");
	}

	urlparser.SetQueryValue(L"categorycode",m_strType);

	CString StrMarket;
	StrMarket.Format(_T("%d"), m_pMerch->m_MerchInfo.m_iMarketId);
	urlparser.SetQueryValue(L"market",StrMarket);
	urlparser.SetQueryValue(L"code", m_pMerch->m_MerchInfo.m_StrMerchCode);

	USES_CONVERSION;
	CString webCode = CConfigInfo::Instance()->GetCodePlatForm();
	urlparser.SetQueryValue(L"webcode",webCode);

	CString StrOrgCode = CConfigInfo::Instance()->GetNewsCenterOrgCode();
	int nOrgCode = 6;
	if(IsAllNumbers(StrOrgCode))
	{
		nOrgCode =_ttoi(StrOrgCode);
	}
	char cOrgCode[6];
	sprintf_s(cOrgCode, sizeof(cOrgCode), "%03d", nOrgCode); 
	char* sha1Code = sha1_hash(cOrgCode);
	urlparser.SetQueryValue(L"orgcode",A2W(sha1Code));

	CString pageUrl = urlparser.GetUrl();
	m_wndCef.OpenUrl(pageUrl);
}

void CDlgWebF10::OnPaint()
{
	CPaintDC dc(this);
}

BOOL CDlgWebF10::OnEraseBkgnd( CDC* pDC )
{
    return TRUE;
}
