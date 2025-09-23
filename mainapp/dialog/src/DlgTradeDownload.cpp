// D:\prj\智能投顾PC客户端\智能投顾PC客户端V2.0.X\main_src\mainapp\dialog\src\DlgTradeDownload.cpp : 实现文件
//

#include "stdafx.h"
#include "GGTong.h"
#include "DlgTradeDownload.h"
#include "DlgTradeSetting.h"
#include "pathfactory.h"
#include "tinyxml.h"
#include "FontFactory.h"
#include "ConfigInfo.h"
#include "UrlParser.h"
#include "DlgQRCode.h"
#include "coding.h"
#include "myzip.h"

/////////////////////////////////////////////////////////////////////////////
// CDlgTradeDownload dialog
#define INVALID_ID			-1
#define WM_SHOWWARNNINGWND           (WM_USER+0x200)

const TCHAR KFontFamily[]			= L"微软雅黑";		//对话框中文本字体
const int32 KGdiFontSize			=17;				//对话框中文本字体高度(适应于gdi)
// 登录用户名或密码错误提示框 的定时器  间隔 3 s
const int32	KiTimerLoginFail			= 5989;				
const int32	KiTimerPeriodLoginFail		= 3000;	

const int32 KiBtnRightSpace				= 8;				// 对话框右侧距离右边框间距值
const int32 KiBtnBottomSpace			= 16;				// 对话框底部距离底部边框间距值
const int32 KiShowTradeNumber			= 6;			    // 每页显示交易程序个数
const int32 KiLeftPos                   = 8;                // 距离右边距离
const int32 KiLeftSpace                 = 6;		        // 图标之间间隔
const int32 KiTopSpace                  = 20;               // 距离caption底部距离
const int32 KiTradeWidth				= 180;              // 图标长度
const int32 KiTradeHeight				= 116;				// 图标高度


IMPLEMENT_DYNAMIC(CDlgTradeDownload, CDialog)

CDlgTradeDownload::CDlgTradeDownload(CWnd* pParent /*=NULL*/)
: CDialog(CDlgTradeDownload::IDD, pParent)
{
	m_iXButtonHovering = INVALID_ID;
	m_pImgMyTrade  = NULL;
	m_wndCef.SetCefBkColor(RGB(255, 255, 255));
	m_rcRegister.SetRect(0, 0, 0, 0);

	m_hArrowCursor=LoadCursor (NULL,IDC_ARROW);
	m_hHandCursor=LoadCursor (NULL,IDC_HAND);
}

CDlgTradeDownload::~CDlgTradeDownload()
{
	DEL(m_pImgMyTrade);
	m_fontStaticText.DeleteObject();
	m_fontCheckText.DeleteObject();
}

void CDlgTradeDownload::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgTradeDownload, CDialog)
	ON_WM_TIMER()
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_MOUSEMOVE()
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave) 
	ON_MESSAGE(UM_DownloadTrade, OnMsgDownloadTrade)
	ON_MESSAGE(WM_SHOWWARNNINGWND, OnMsgShowWarnningWnd)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()


// CDlgTradeDownload 消息处理程序

BOOL CDlgTradeDownload::OnInitDialog() 
{
	m_pImgMyTrade  = Image::FromFile(L"image//Broker//btnMyTrade.png");
	
	DrawClientButton();

	return TRUE; 
}

BOOL CDlgTradeDownload::OnEraseBkgnd(CDC* pDC)
{
	CDC memDC;
	memDC.CreateCompatibleDC(pDC);
	CBitmap bmp;
	CRect rcWindow;
	GetWindowRect(&rcWindow);
	rcWindow.OffsetRect(-rcWindow.left, -rcWindow.top);

	bmp.CreateCompatibleBitmap(pDC, rcWindow.Width(), rcWindow.Height());
	memDC.SelectObject(&bmp);
	memDC.FillSolidRect(0, 0, rcWindow.Width(), rcWindow.Width(), RGB(255,255,255));
	memDC.SetBkMode(TRANSPARENT);
	Gdiplus::Graphics graphics(memDC.GetSafeHdc());

	// 文本
	{

		RectF grectRegister;
		grectRegister.X = (REAL)66;
		grectRegister.Y = (REAL)rcWindow.bottom - 45;
		grectRegister.Width = (REAL)80;
		grectRegister.Height = (REAL)30;

		RectF grectRecommend;
		grectRecommend.X = (REAL)66;
		grectRecommend.Y = (REAL)rcWindow.top + 45;
		grectRecommend.Width = (REAL)80;
		grectRecommend.Height = (REAL)30;

		//绘制文字
		StringFormat strFormat;
		strFormat.SetAlignment(StringAlignmentCenter);
		strFormat.SetLineAlignment(StringAlignmentCenter);

		typedef struct T_NcFont 
		{
		public:
			CString	m_StrName;
			float   m_Size;
			int32	m_iStyle;

		}T_NcFont;
		T_NcFont  m_fontRegister, m_fontRecommend;


		m_fontRegister.m_StrName = gFontFactory.GetExistFontName(L"微软雅黑");//...
		m_fontRegister.m_Size	 = 9;
		m_fontRegister.m_iStyle	 = FontStyleRegular | FontStyleUnderline;	
		Gdiplus::FontFamily fontFamilyRegister(m_fontRegister.m_StrName);
		Gdiplus::Font fontRegister(&fontFamilyRegister, m_fontRegister.m_Size, m_fontRegister.m_iStyle, UnitPoint);


		m_fontRecommend.m_StrName = gFontFactory.GetExistFontName(L"微软雅黑");//...
		m_fontRecommend.m_Size	 = 12;
		m_fontRecommend.m_iStyle	 = FontStyleBold;	
		Gdiplus::FontFamily fontFamilyReCommend(m_fontRecommend.m_StrName);
		Gdiplus::Font fontRecommend(&fontFamilyReCommend, m_fontRecommend.m_Size, m_fontRecommend.m_iStyle, UnitPoint);


		RectF rcBound;
		PointF point;

		CString StrRegister  = L"没有开户?立即开户或者转户";
		CString StrRecommend = L"推荐券商";



//		graphics.MeasureString(StrRegister, StrRegister.GetLength(), &fontRegister, point, &strFormat, &rcBound);
//		grectRegister.Width = rcBound.Width;

// 		graphics.MeasureString(StrRecommend, StrRecommend.GetLength(), &fontRecommend, point, &strFormat, &rcBound);
// 		grectRecommend.Width = rcBound.Width;

		m_rcRegister.top    = grectRegister.Y;
		m_rcRegister.left   = grectRegister.X;
		m_rcRegister.right  = m_rcRegister.left + grectRegister.Width;
		m_rcRegister.bottom = m_rcRegister.top  + grectRegister.Height;

		
		SolidBrush brush((ARGB)Color::White);

// 		// 绘制推荐券商
// 		brush.SetColor(Color::Color(0, 0, 0));
// 		graphics.DrawString(StrRecommend, StrRecommend.GetLength(), &fontRecommend, grectRecommend, &strFormat, &brush);

		// 绘制注册文字
//		brush.SetColor(Color::Color(102, 102, 102));
//		graphics.DrawString(StrRegister, StrRegister.GetLength(), &fontRegister, grectRegister, &strFormat, &brush);
	
	}

	CRect rcPaint;
	pDC->GetClipBox(&rcPaint);
	map<int, CNCButton>::iterator iter;
	CRect rcControl;

	for (iter=m_mapBtn.begin(); iter!=m_mapBtn.end(); ++iter)
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

	pDC->BitBlt(0, 0, rcWindow.Width(), rcWindow.Height(), &memDC, 0, 0, SRCCOPY);
	pDC->SelectClipRgn(NULL);

	memDC.DeleteDC();
	bmp.DeleteObject();

	return TRUE; 
}

void CDlgTradeDownload::OnMouseMove(UINT nFlags, CPoint point) 
{
	int32 iButton = TButtonHitTest(point);

	if (iButton != m_iXButtonHovering)
	{
		if (INVALID_ID != m_iXButtonHovering)
		{
// 			if (ID_TRADESETTING_CLOSE==iButton )
// 			{
// 				m_mapBtn[m_iXButtonHovering].MouseLeave(FALSE);
// 				DrawTitleBar();
// 			}
// 			else
			{
				m_mapBtn[m_iXButtonHovering].MouseLeave();
			}
			m_iXButtonHovering = INVALID_ID;
		}

		if (INVALID_ID != iButton)
		{	
			m_iXButtonHovering = iButton;
			// 			if (ID_TRADESETTING_CLOSE==iButton )
			// 			{
			// 				m_mapBtn[m_iXButtonHovering].MouseHover(FALSE);
			// 				DrawTitleBar();
			// 			}
			// 			else
			{
				m_mapBtn[m_iXButtonHovering].MouseHover();
			}
		}
	}

	// 设置鼠标形状
// 	if (m_rcRegister.PtInRect(point))
// 	{
// 		::SetCursor(m_hHandCursor);
// 	}
// 	else
// 	{
// 		::SetCursor(m_hArrowCursor);
// 	}

	// 响应 WM_MOUSELEAVE消息
	TRACKMOUSEEVENT csTME;
	csTME.cbSize	= sizeof (csTME);
	csTME.dwFlags	= TME_LEAVE;
	csTME.hwndTrack = m_hWnd ;		// 指定要追踪的窗口 
	::_TrackMouseEvent (&csTME);	// 开启Windows的WM_MOUSELEAVE事件支持 

	CDialog::OnMouseMove(nFlags, point);
}

LRESULT CDlgTradeDownload::OnMouseLeave(WPARAM wParam, LPARAM lParam)       
{     
	if (INVALID_ID != m_iXButtonHovering)
	{
// 		if (ID_TRADESETTING_CLOSE==m_iXButtonHovering )
// 		{
// 			m_mapBtn[m_iXButtonHovering].MouseLeave(FALSE);
// 			DrawTitleBar();
// 		}
// 		else
		{
			m_mapBtn[m_iXButtonHovering].MouseLeave();
		}
		m_iXButtonHovering = INVALID_ID;
	}

	return 0;       
} 

void CDlgTradeDownload::OnLButtonDown(UINT nFlags, CPoint point) 
{
// 	if (m_rcRegister.PtInRect(point))
// 	{
//  	UrlParser urlPaser(L"alias://brokerReg");
//  	CString StrUrl = urlPaser.GetUrl();
//  		
//  	ShellExecute(0, L"open", StrUrl, NULL, NULL, SW_NORMAL);
// 		
// 		CDlgQRCode::ShowPopupQRCode();
// 		
// 		return;
// 	}
	int32 iButton = TButtonHitTest(point);
	if (INVALID_ID != iButton)
	{
// 		if (ID_TRADESETTING_CLOSE==iButton )
// 		{
// 			m_mapBtn[iButton].LButtonDown(FALSE);
// 			DrawTitleBar();
// 		}
// 		else
		{
			m_mapBtn[iButton].LButtonDown();
		}
	}
}

void CDlgTradeDownload::OnLButtonUp(UINT nFlags, CPoint point) 
{
	int32 iButton = TButtonHitTest(point);
	if (INVALID_ID != iButton)
	{
		m_mapBtn[iButton].LButtonUp();
	}
}

BOOL CDlgTradeDownload::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	CGGTongApp* pApp = DYNAMIC_DOWNCAST(CGGTongApp, AfxGetApp());
	int32 iID = (int32)wParam;
	std::map<int, CNCButton>::iterator it = m_mapBtn.find(iID);


	if (m_mapBtn.end() != it)
	{
		switch (iID)
		{
		case IDC_BTN_MYTRADE:
			{
				CDlgTradeSetting *pMainDlg = (CDlgTradeSetting*)GetParent();
				pMainDlg->ShowPage(0);//跳到setup对话框
			}
			break;
		default:
			{
			}
			break;
		}
	}

	return CDialog::OnCommand(wParam, lParam);
}

void CDlgTradeDownload::OnPaint() 
{
	CPaintDC dc(this);

}

void CDlgTradeDownload::AddButton(LPRECT lpRect, Image *pImage, UINT nCount, UINT nID, LPCTSTR lpszCaption)
{
	ASSERT(pImage);
	CNCButton btnControl; 
	btnControl.CreateButton(lpszCaption, lpRect, this, pImage, nCount, nID);
	btnControl.SetParentFocus(FALSE);

	CNCButton::T_NcFont m_Font;
	m_Font.m_StrName = gFontFactory.GetExistFontName(L"微软雅黑");
	m_Font.m_Size	 = 10;
	m_Font.m_iStyle	 = FontStyleRegular;	
	btnControl.SetFont(m_Font);

	m_mapBtn[nID] = btnControl;
}

int	 CDlgTradeDownload::TButtonHitTest(CPoint point)
{
	map<int, CNCButton>::iterator iter;

	// 遍历所有按钮
	for (iter=m_mapBtn.begin(); iter!=m_mapBtn.end(); ++iter)
	{
		CNCButton &btnControl = iter->second;

		// 点point是否在已绘制的按钮区域内
		if (btnControl.PtInButton(point) && btnControl.IsEnable())
		{
			return btnControl.GetControlId();
		}
	}

	return INVALID_ID;
}

BOOL CDlgTradeDownload::PreTranslateMessage(MSG* pMsg) 
{
	return BaseDialog::PreTranslateMessage(pMsg);
}
void CDlgTradeDownload::DrawClientButton()
{
	CGGTongApp *pApp = DYNAMIC_DOWNCAST(CGGTongApp, AfxGetApp());
	
	if (NULL == pApp || NULL == pApp->m_pConfigInfo)
	{
		return;
	}

	int32 iTitleHeight = 60;
	int32 iWndWidth = 570;
	int32 iWndHeight = 340;
	int32 iTopHeight = 209;
	CRect rcWnd(0, 0, iWndWidth, iWndHeight);
	CString StrUrl = L"";
	CRect rcTmp;
	//MoveWindow(rcWnd);
	CWindowDC dc(this);
	Graphics graphics(dc.GetSafeHdc());

	m_mapBtn.clear();
	
	rcTmp.left  = rcWnd.left  + 60;
	rcTmp.right = rcWnd.right - 60;
	rcTmp.top   = rcWnd.top + 38;
	rcTmp.bottom = rcWnd.bottom - 80;
	
	UrlParser urlParser(L"alias://tradeSet");
	StrUrl = urlParser.GetUrl();
	m_wndCef.CreateWndCef(9877,this, rcTmp,StrUrl);

//	m_wndCef.SetUrl(StrUrl);
//	m_wndCef.Create(NULL,NULL, WS_CHILD|WS_VISIBLE, rcTmp, this, 9877);
	
	rcTmp.left = rcWnd.right - m_pImgMyTrade->GetWidth() - KiBtnRightSpace -30;
	rcTmp.top = rcWnd.bottom - m_pImgMyTrade->GetHeight()/3 -KiBtnBottomSpace;
	rcTmp.right = rcTmp.left + m_pImgMyTrade->GetWidth();
	rcTmp.bottom = rcTmp.top + m_pImgMyTrade->GetHeight()/3;
	AddButton(rcTmp, m_pImgMyTrade, 3, IDC_BTN_MYTRADE, L"我的交易");	// 我的交易	
}

LRESULT CDlgTradeDownload::OnMsgDownloadTrade(WPARAM wParam, LPARAM lParam)
{
	CGGTongApp* pApp = DYNAMIC_DOWNCAST(CGGTongApp, AfxGetApp());
	CString *Str = (CString*)wParam;
	CString StrJsonValue = *Str;
	CDlgTradeSetting *pMainDlg = (CDlgTradeSetting*)GetParent();
 	if (!pMainDlg->m_pDlgTradeSetup->GetDownloadStatusFlag())
 	{
		PostMessage(WM_SHOWWARNNINGWND, 0, 0);

		return 0;
 	}

	if (pApp && pApp->m_pConfigInfo && pMainDlg)
	{
		T_TradeCfgParam tradeCfg;
		ParseJson2TradeCfgParam(StrJsonValue, tradeCfg);

		pMainDlg->m_pDlgTradeSetup->AddTradeApp(tradeCfg);
		pMainDlg->ShowPage(0);//跳到Setup对话框
	}
	return 0;
}

LRESULT CDlgTradeDownload::OnMsgShowWarnningWnd(WPARAM wParam, LPARAM lParam)
{
	AfxMessageBox(L"当前正在下载，请稍候...");

	return 0;
}

void CDlgTradeDownload::ParseJson2TradeCfgParam(CString StrJsonValue, T_TradeCfgParam &stTrade)
{
	// json解析
	Json::Reader jsonReader;
	Json::Value	 jsonValue;
	string sFilePath = _Unicode2MultiChar(StrJsonValue);
	const char* strFilePath = sFilePath.c_str();
	
	if (jsonReader.parse(strFilePath, jsonValue))
	{
		if (jsonValue.isNull())
		{
			return;
		}

		stTrade.StrBrokerName = jsonValue["StrBrokerName"].asCString();
		stTrade.StrAppName    = jsonValue["StrAppName"].asCString();
		stTrade.StrShortcut   = jsonValue["StrShortcut"].asCString();
		stTrade.iType         =	atoi(jsonValue["iType"].asCString());
		stTrade.StrRun        = jsonValue["StrRun"].asCString();
		stTrade.StrUrl        = jsonValue["StrUrl"].asCString();
		stTrade.StrLogo        = jsonValue["StrLogo"].asCString();
	}
	else
	{
		return ;
	}
}