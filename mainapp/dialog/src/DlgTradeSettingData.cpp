// DlgTradeSettingData.cpp : 实现文件
//

#include "stdafx.h"
#include <gdiplus.h>
#include <UrlMon.h>
#include "GGTong.h"
#include "FontFactory.h"
#include "PathFactory.h"
#include "myzip.h"
#include "coding.h"
#include "DlgTradeSettingData.h"
#include "WebClient.h"

using namespace Gdiplus;


#pragma comment(lib, "urlmon.lib")


#define INVALID_ID						-1
#define ID_TRADESETTING_DATA_CLOSE		30001 
#define ID_TRADESETTING_DATA_DEFAULT	30002 
#define ID_TRADESETTING_DATA_LOGIN		30003 

#define BACKGROUND_COLOR RGB(238,240,248)
#define COLOR_GREEN  RGB(39,142,82)
#define COLOR_RED    RGB(255,0,0)
#define COLOR_BLACK	 RGB(0,0,0)
#define COLOR_WHITE	 RGB(255,255,255)

#define WM_TRADESETTING_MSGBEGIN		  (WM_USER+0x200)
#define WM_TRADESETTING_SETPOS			  (WM_USER+0x201)
#define WM_TRADESETTING_CLOSECHILDWND	  (WM_USER+0x202)
#define WM_TRADESETTING_SETDEFAULT		  (WM_USER+0x203)
#define WM_TRADESETTING_CHANGEIDLESTATUS  (WM_USER+0x204)
#define WM_TRADESETTING_HIDEWND           (WM_USER+0x205)


const int32 KiWndWidth  =  180;		// 对话框宽度
const int32 KiWndHeight =  116;		// 对话框高度
// CDlgTradeSettingData 对话框

IMPLEMENT_DYNAMIC(CDlgTradeSettingData, CDialog)

CDlgTradeSettingData::CDlgTradeSettingData(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgTradeSettingData::IDD, pParent)
{
	m_pImgBkg           = NULL;
	m_pImgDefaultBkg    = NULL;
	m_pImgBrokerLogo    = NULL;
	m_pImgMoreTradeBk   = NULL;
	m_pImgDefaultBrokerLog = NULL;
	m_pImgTradeExit  = NULL;
	m_pImgDefaultBtn = NULL;
	m_pImgLoginBtn   = NULL;	

	
	m_bIsMoreTrade      = FALSE;
	
	m_iXButtonHovering  = INVALID_ID;
	m_hDownloadHandle   = NULL;
	
	
	LOGFONT lf = {0};
	lf.lfHeight = 12;
	lf.lfWeight = FW_BOLD;
	_tcscpy(lf.lfFaceName, _T("微软雅黑"));
	m_fontContent.CreateFontIndirect(&lf);

}

CDlgTradeSettingData::~CDlgTradeSettingData()
{
	DEL(m_pImgBkg);
	DEL(m_pImgDefaultBkg);
	DEL(m_pImgBrokerLogo);
	DEL(m_pImgMoreTradeBk);
	DEL(m_pImgDefaultBrokerLog);
	DEL(m_pImgTradeExit);  
	DEL(m_pImgDefaultBtn); 
	DEL(m_pImgLoginBtn);   
	
	StopDownloadTrade();
}

void CDlgTradeSettingData::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_TRADENAME, m_staticTradeName);
	DDX_Control(pDX, IDC_STATIC_APPTYPE, m_staticAppType);
	DDX_Control(pDX, IDC_PROGRESS_DOWNLOAD, m_ProgressDownload);
}


BEGIN_MESSAGE_MAP(CDlgTradeSettingData, CDialog)
	ON_WM_PAINT()
	ON_WM_NCHITTEST()
	ON_WM_MOUSEMOVE()
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave) 
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_MESSAGE(WM_TRADESETTING_SETPOS, OnMsgProgressSetPos)
END_MESSAGE_MAP()


// CDlgTradeSettingData 消息处理程序

BOOL CDlgTradeSettingData::OnInitDialog()
{
	CDialog::OnInitDialog();

	CRect rcWnd(0, 0, KiWndWidth, KiWndHeight);

	m_pImgBkg         = Image::FromFile(L"./image/Broker/Bkg.png");
	m_pImgDefaultBkg  = Image::FromFile(L"./image/Broker/DefaultBkg.png");
	m_pImgMoreTradeBk = Image::FromFile(L"./image/Broker/MoreBK.png");
	m_pImgDefaultBrokerLog = Image::FromFile(L"./image/Broker/brokerLogo.bmp");
	m_pImgTradeExit   = Image::FromFile(L"./image/Broker/tradeExit.png");
	m_pImgDefaultBtn  = Image::FromFile(L"./image/Broker/btnDefault.png");
	m_pImgLoginBtn    = Image::FromFile(L"./image/Broker/btnLogin.png");
	
	int iHeight = rcWnd.top + 6;
	CRect rcTmp(rcWnd.right-m_pImgTradeExit->GetWidth()-5, iHeight, rcWnd.right-5, iHeight + m_pImgTradeExit->GetHeight());
	AddButton(rcTmp, m_pImgTradeExit, 1, ID_TRADESETTING_DATA_CLOSE);	// 关闭按钮

	int32 iLeftPos = 64;
	int32 iTopPos  = 81;
	CRect rcDefault(iLeftPos, iTopPos, iLeftPos + m_pImgDefaultBtn->GetWidth(), iTopPos + m_pImgDefaultBtn->GetHeight()/4);
	AddButton(rcDefault, m_pImgDefaultBtn, 4, ID_TRADESETTING_DATA_DEFAULT, L"默认");	// 默认按钮

	iLeftPos = iLeftPos + m_pImgDefaultBtn->GetWidth() + 6;
	
	CRect rcLogin(iLeftPos, iTopPos, iLeftPos + m_pImgLoginBtn->GetWidth(), iTopPos + m_pImgLoginBtn->GetHeight()/3);
	AddButton(rcLogin, m_pImgLoginBtn, 3, ID_TRADESETTING_DATA_LOGIN, L"登录");	// 登录按钮
	
	iTopPos = iTopPos + m_pImgLoginBtn->GetHeight()/3 + 10;
	CRect rcProgress(0, iTopPos, KiWndWidth, iTopPos + 5);
	GetDlgItem(IDC_PROGRESS_DOWNLOAD)->MoveWindow(rcProgress);
	GetDlgItem(IDC_PROGRESS_DOWNLOAD)->ShowWindow(SW_HIDE);
	
	return TRUE; 
}

void CDlgTradeSettingData::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	CRect rt;
	GetClientRect(rt);

	Graphics graphic(dc.GetSafeHdc());
	// 背景
	if (m_bIsMoreTrade)
	{
		RectF rtDefaultBk(0, 0, KiWndWidth, KiWndHeight);
		graphic.DrawImage(m_pImgMoreTradeBk, rtDefaultBk, 0, 0, m_pImgMoreTradeBk->GetWidth(), m_pImgMoreTradeBk->GetHeight(), UnitPixel);	
		
		m_staticAppType.ShowWindow(SW_HIDE);
		m_staticTradeName.ShowWindow(SW_HIDE);
		return;
	}
	else
	{
		if (m_stTradeCfgData.bDefaultTrade)
		{
			if (m_pImgDefaultBkg)
			{
				RectF rtDefaultBk(0, 0, KiWndWidth, KiWndHeight);
				graphic.DrawImage(m_pImgDefaultBkg, rtDefaultBk, 0, 0, m_pImgDefaultBkg->GetWidth(), m_pImgDefaultBkg->GetHeight(), UnitPixel);
			}
		} 
		else
		{
			if (m_pImgBkg)
			{
				RectF rtBk(0, 0, KiWndWidth, KiWndHeight);
				graphic.DrawImage(m_pImgBkg, rtBk, 0, 0, m_pImgBkg->GetWidth(), m_pImgBkg->GetHeight(), UnitPixel);
			}
		}
	}
	
	// 券商图标
	if (m_pImgBrokerLogo && m_pImgBrokerLogo->GetLastStatus() == Ok)
	{
		RectF rtCountry(4, 36, 45, 45);
		graphic.DrawImage(m_pImgBrokerLogo, rtCountry, 0, 0, m_pImgBrokerLogo->GetWidth(), m_pImgBrokerLogo->GetHeight(), UnitPixel);
	}
	else
	{
		RectF rtCountry(4, 36, 45, 45);
		graphic.DrawImage(m_pImgDefaultBrokerLog, rtCountry, 0, 0, m_pImgDefaultBrokerLog->GetWidth(), m_pImgDefaultBrokerLog->GetHeight(), UnitPixel);
	}

	// 券商名称
	if (!m_stTradeCfgData.StrBrokerName.IsEmpty())
	{
		SolidBrush  brush(Color(255, 0,0,0));
		FontFamily  fontFamily(L"微软雅黑");
		Gdiplus::Font font(&fontFamily, 15, FontStyleBold, UnitPixel);
		StringFormat stringFormat;
		stringFormat.SetLineAlignment(StringAlignmentCenter);

		CSize size = dc.GetTextExtent(m_stTradeCfgData.StrBrokerName);
		int cy = 55 - size.cy - 5;
		int cx = 62;
		int pos = rt.Width()-73;
		if (size.cx > pos)
		{
			cy = 55 - size.cy * 2 -5;
			RectF rect(cx, cy, 15 * 18, size.cy * 2);
			graphic.DrawString(m_stTradeCfgData.StrBrokerName, -1, &font, rect, &stringFormat, &brush);
		}
		else
		{
			PointF pt(cx, cy);
			graphic.DrawString(m_stTradeCfgData.StrBrokerName, m_stTradeCfgData.StrBrokerName.GetLength(), &font, pt, &brush);
		}
	}

	CRect rcPaint;
	dc.GetClipBox(&rcPaint);
	map<int, CNCButton>::iterator iter;
	CRect rcControl;

	for (iter=m_mapBtn.begin(); iter!=m_mapBtn.end(); ++iter)
	{
		CNCButton &btnControl = iter->second;
		btnControl.GetRect(rcControl);
		if (ID_TRADESETTING_DATA_DEFAULT == btnControl.GetControlId())
		{
			btnControl.EnableButton(!m_stTradeCfgData.bDefaultTrade);
		}
		else if (ID_TRADESETTING_DATA_LOGIN == btnControl.GetControlId())
		{
			btnControl.EnableButton(m_stTradeCfgData.bDldComplete);
		}
		// 判断当前按钮是否需要重绘
		if (!rcPaint.IsRectEmpty() && !CRect().IntersectRect(&rcControl, rcPaint))
		{
			continue;
		}

		btnControl.DrawButton(&graphic);
	}
}

BOOL CDlgTradeSettingData::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	int32 iID = (int32)wParam;
	CGGTongApp *pApp   = DYNAMIC_DOWNCAST(CGGTongApp, AfxGetApp());
	std::map<int, CNCButton>::iterator it = m_mapBtn.find(iID);
	CString StrRunPath = L"";
	
	if (m_mapBtn.end() != it)
	{
		switch (iID)
		{
		case ID_TRADESETTING_DATA_CLOSE:
			{	
				GetParent()->PostMessage(WM_TRADESETTING_CLOSECHILDWND,(WPARAM)this, 0 );
			}
			break;
		case ID_TRADESETTING_DATA_DEFAULT:
			{
				GetParent()->PostMessage(WM_TRADESETTING_SETDEFAULT,(WPARAM)this, 0 );
			}
			break;
		case ID_TRADESETTING_DATA_LOGIN:
			{
				if (pApp)
				{
					StrRunPath.Format(L".\\private\\%s\\trade\\%s\\%s", pApp->m_pDocument->m_pAbsCenterManager->GetUserName(), m_stTradeCfgData.StrShortcut,m_stTradeCfgData.StrRun);
				
					ShellExecute(NULL,L"open",StrRunPath,NULL,NULL,SW_SHOWNORMAL);
					// 打开交易程序后关闭交易设置对话框
					GetParent()->PostMessage(WM_TRADESETTING_HIDEWND, (WPARAM)0, (LPARAM)0 );
				}
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

void CDlgTradeSettingData::InitData()
{
	// 交易软件名称
	CString StrText = m_stTradeCfgData.StrAppName;

	m_staticTradeName.SetTitleText1(StrText);
	m_staticTradeName.SetTitleColor1(COLOR_BLACK);
	m_staticTradeName.SetBkColor(BACKGROUND_COLOR);
	

	m_staticAppType.SetTitleText1(L"股票");
	m_staticAppType.SetTitleColor1(COLOR_WHITE);
	m_staticAppType.SetBkColor(RGB(254,178,59));
	m_staticAppType.SetAlign(DT_CENTER | DT_VCENTER);
	
	// 下载券商logo
	DownloadBrokerLog();

	// 如果是新添加的需要下载
	if (m_stTradeCfgData.bNewAdd)
	{
		GetDlgItem(IDC_PROGRESS_DOWNLOAD)->ShowWindow(SW_SHOW);
		StartDownloadTrade();
	}
}

void CDlgTradeSettingData::DownloadBrokerLog()
{
	CGGTongApp *pApp = DYNAMIC_DOWNCAST(CGGTongApp, AfxGetApp());
	CString    StrBrokerLogoUrl = L"", StrSavePath = L"";
	if (pApp)
	{
		UrlParser urlParser(L"alias://tradeDownload");
		StrBrokerLogoUrl.Format(L"%s/logo/%s", urlParser.GetUrl(), m_stTradeCfgData.StrLogo);
		StrSavePath.Format(L".\\image\\Broker\\%s",m_stTradeCfgData.StrLogo);

		WebClient webClient;
		webClient.SetTimeouts(100, 200);
		webClient.DownloadFile(StrBrokerLogoUrl, StrSavePath);
	}
}

void CDlgTradeSettingData::LoadBrokerIcon(CString strFileName)
{
	m_pImgBrokerLogo = Image::FromFile(strFileName);
}

void CDlgTradeSettingData::AddButton(LPRECT lpRect, Image *pImage, UINT nCount, UINT nID, LPCTSTR lpszCaption)
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
	if (ID_TRADESETTING_DATA_DEFAULT == nID)
	{
		btnControl.SetTextColor(RGB(0,0,0), RGB(0,0,0), RGB(0,0,0));
	}
	else if (ID_TRADESETTING_DATA_LOGIN == nID)
	{
		btnControl.SetTextColor(RGB(255,109,0), RGB(255,109,0), RGB(255,109,0));
	}

	m_mapBtn[nID] = btnControl;
}

int	 CDlgTradeSettingData::TButtonHitTest(CPoint point)
{
	if(m_bIsMoreTrade)
	{
		return INVALID_ID;
	}

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

void CDlgTradeSettingData::OnMouseMove(UINT nFlags, CPoint point) 
{
	int32 iButton = TButtonHitTest(point);

	if (iButton != m_iXButtonHovering)
	{
		if (INVALID_ID != m_iXButtonHovering)
		{
			if (ID_TRADESETTING_DATA_CLOSE==iButton )
			{
				m_mapBtn[m_iXButtonHovering].MouseLeave(FALSE);
			}
			else
			{
				m_mapBtn[m_iXButtonHovering].MouseLeave();
			}
			m_iXButtonHovering = INVALID_ID;
		}

		if (INVALID_ID != iButton)
		{	
			m_iXButtonHovering = iButton;
			if (ID_TRADESETTING_DATA_CLOSE==iButton )
			{
				m_mapBtn[m_iXButtonHovering].MouseHover(FALSE);
			}
			else
			{
				m_mapBtn[m_iXButtonHovering].MouseHover();
			}
		}
	}

	// 响应 WM_MOUSELEAVE消息
	TRACKMOUSEEVENT csTME;
	csTME.cbSize	= sizeof (csTME);
	csTME.dwFlags	= TME_LEAVE;
	csTME.hwndTrack = m_hWnd ;		// 指定要追踪的窗口 
	::_TrackMouseEvent (&csTME);	// 开启Windows的WM_MOUSELEAVE事件支持 

	CDialog::OnMouseMove(nFlags, point);
}

LRESULT CDlgTradeSettingData::OnMouseLeave(WPARAM wParam, LPARAM lParam)       
{     
	if (INVALID_ID != m_iXButtonHovering)
	{
		if (ID_TRADESETTING_DATA_CLOSE==m_iXButtonHovering )
		{
			m_mapBtn[m_iXButtonHovering].MouseLeave(FALSE);
		}
		else
		{
			m_mapBtn[m_iXButtonHovering].MouseLeave();
		}
		m_iXButtonHovering = INVALID_ID;
	}

	return 0;       
} 

void CDlgTradeSettingData::OnLButtonDown(UINT nFlags, CPoint point) 
{
	int32 iButton = TButtonHitTest(point);
	if (INVALID_ID != iButton)
	{
		if (ID_TRADESETTING_DATA_CLOSE==iButton )
		{
			m_mapBtn[iButton].LButtonDown(FALSE);
		}
		else
		{
			m_mapBtn[iButton].LButtonDown();
		}
	}
	else if( INVALID_ID == iButton && m_bIsMoreTrade)
	{
		
		GetParent()->PostMessage(WM_TRADESETTING_MSGBEGIN,(WPARAM)FALSE, 0 );
	}
}

void CDlgTradeSettingData::OnLButtonUp(UINT nFlags, CPoint point) 
{
	int32 iButton = TButtonHitTest(point);
	if (INVALID_ID != iButton)
	{
		if (ID_TRADESETTING_DATA_CLOSE==iButton )
		{
			m_mapBtn[iButton].LButtonUp(FALSE);
		}
		else
		{
			m_mapBtn[iButton].LButtonUp();
		}
	}
}

void CDlgTradeSettingData::StopDownloadTrade()
{
	if (NULL != m_hDownloadHandle)
	{
		if (::WaitForSingleObject(m_hDownloadHandle, 0) == WAIT_TIMEOUT)
		{
			::TerminateThread(m_hDownloadHandle, -1);
		}
		CloseHandle(m_hDownloadHandle);
		m_hDownloadHandle = NULL;
	}
}

void CDlgTradeSettingData::StartDownloadTrade()
{
	StopDownloadTrade();

	m_hDownloadHandle = (HANDLE)_beginthreadex(NULL, 0, DownloadTradeThread, this, 0, NULL);
}

unsigned int __stdcall CDlgTradeSettingData::DownloadTradeThread(void *pParam)
{
	CDlgTradeSettingData *pThis = (CDlgTradeSettingData *)pParam;
	return (unsigned int)pThis->RunDownloadTrade();
}

int CDlgTradeSettingData::RunDownloadTrade()
{
	CGGTongApp* pApp = DYNAMIC_DOWNCAST(CGGTongApp, AfxGetApp());
	CMyZip myZip;

	if (!pApp || !pApp->m_pConfigInfo)
	{
		return 0;
	}

	CString StrSavePath = L"", StrPrivateTrade = L"", StrTradeUrl = L"";
	

	UrlParser urlParser(L"alias://tradeDownload");
	StrSavePath.Format(L".\\zip\\trade\\%s\\", m_stTradeCfgData.StrShortcut);
	StrTradeUrl.Format(L"%s/%s/%s", urlParser.GetUrl(), m_stTradeCfgData.StrShortcut,m_stTradeCfgData.StrUrl);
	StrPrivateTrade.Format(L".\\private\\%s\\trade\\%s\\",pApp->m_pDocument->m_pAbsCenterManager->GetUserName(),m_stTradeCfgData.StrShortcut);
	
	_tcheck_if_mkdir(StrSavePath.GetBuffer(MAX_PATH));
	StrSavePath.ReleaseBuffer();

	StrSavePath += m_stTradeCfgData.StrUrl;
	
	// Url增加随机数处理防止从缓存读取下载数据
	CString StrRandText = L"";
	StrRandText.Format(L"?rd=%d",time(NULL));	// 生成随机URL
	StrTradeUrl += StrRandText;
	
	// 查找该压缩包是否已经下载过了
	CFileFind fileFind; 
	if ( fileFind.FindFile(StrSavePath) ) 
	{ 
		// 找到压缩文件直接解压就行了
	 	myZip.Zip_UnPackFiles(StrSavePath, StrPrivateTrade,GetSafeHwnd());
		m_stTradeCfgData.bDldComplete = true;

		return 1; 
	} 

	HRESULT hr = URLDownloadToFile(NULL, StrTradeUrl, StrSavePath, NULL, this);
	if (hr==S_OK)
	{
		myZip.Zip_UnPackFiles(StrSavePath, StrPrivateTrade,GetSafeHwnd());

		m_stTradeCfgData.bNewAdd      = false;
		m_stTradeCfgData.bDldComplete = true;
	}
	else if (hr== E_OUTOFMEMORY)
	{
		AfxMessageBox(L"缓冲区长度无效，或内存不足，无法完成操作！", MB_OK | MB_ICONERROR);
	}
	else if (hr== INET_E_DOWNLOAD_FAILURE)
	{
		AfxMessageBox(L"指定的资源或回调接口无效！", MB_OK | MB_ICONERROR);
	}

	return 1;
}

// 下载回调函数 
LRESULT CDlgTradeSettingData::OnProgress(ULONG ulProgress,
								   ULONG ulProgressMax,
								   ULONG ulSatusCode,
								   LPCWSTR szStatusText)
{
	int32 iProgressMax  =  ulProgressMax / 2048;
	int32 iProgress     =  ulProgress    / 2048;
	m_ProgressDownload.SetRange(0, iProgressMax);
	m_ProgressDownload.SetPos(iProgress);

	return S_OK;
}

LRESULT CDlgTradeSettingData::OnMsgProgressSetPos(WPARAM wParam, LPARAM lParam)
{
	int32 iRange = (int32)lParam;
	int32 iPos   = (int32)wParam;
	if (!m_ProgressDownload.IsWindowVisible())
	{
		GetDlgItem(IDC_PROGRESS_DOWNLOAD)->ShowWindow(SW_SHOW);
	}
	else if ( iRange - 1 == iPos )
	{
		GetDlgItem(IDC_PROGRESS_DOWNLOAD)->ShowWindow(SW_HIDE);
		GetParent()->PostMessage(WM_TRADESETTING_CHANGEIDLESTATUS, (WPARAM)this, (LPARAM)true);
	}

 	m_ProgressDownload.SetRange(0, iRange);
	m_ProgressDownload.SetPos(iPos);

	return 0;
}
