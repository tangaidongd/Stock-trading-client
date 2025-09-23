// D:\prj\智能投顾PC客户端\智能投顾PC客户端V2.0.X\main_src\mainapp\dialog\src\DlgTradeSetup.cpp : 实现文件
//

#include "stdafx.h"
#include "GGTong.h"
#include <io.h>
#include "DlgTradeSetup.h"
#include "pathfactory.h"
#include "tinyxml.h"
#include "FontFactory.h"
#include "ConfigInfo.h"
#include "ShareFun.h"
#include "coding.h"
#include "myzip.h"
#include <UrlMon.h>
#include "Wininet.h"
#include "DlgTradeSetting.h"
#include "XmlShare.h"

#pragma comment(lib, "urlmon.lib")
#pragma comment(lib, "Wininet.lib")

/////////////////////////////////////////////////////////////////////////////
// CDlgTradeSetup dialog
#define INVALID_ID			-1

#define WM_TRADESETTING_MSGBEGIN		  (WM_USER+0x200)
#define WM_TRADESETTING_SETPOS			  (WM_USER+0x201)
#define WM_TRADESETTING_CLOSECHILDWND	  (WM_USER+0x202)
#define WM_TRADESETTING_SETDEFAULT		  (WM_USER+0x203)
#define WM_TRADESETTING_CHANGEIDLESTATUS  (WM_USER+0x204)
#define WM_TRADESETTING_HIDEWND           (WM_USER+0x205)

//////////////////////////////////////////////////////////////////////////
const TCHAR KStrTBWndSection[]		= _T("TBWnd");
const TCHAR KStrTBWndTradeKey[]		= _T("trade");
//////////////////////////////////////////////////////////////////////////

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

static const char* KE_Trade         = "trade";
static const char* KE_TradeItem     = "tradeItem";
static const char* KA_BrokerName    = "brokerName";
static const char* KA_AppName       = "appName";
static const char* KA_Shortcut      = "shortcut";
static const char* KA_Type          = "type";
static const char* KA_Run           = "run";
static const char* KA_Url           = "url";
static const char* KA_Logo          = "logo";
static const char* KA_DefaultSel    = "default";
static const char* KA_IsDownload    = "isDownload";

bool  Compare(CDlgTradeSettingData * pData1, CDlgTradeSettingData *pData2)
{
	return pData1->m_stTradeCfgData.bDefaultTrade > pData2->m_stTradeCfgData.bDefaultTrade;
}

IMPLEMENT_DYNAMIC(CDlgTradeSetup, CDialog)

CDlgTradeSetup::CDlgTradeSetup(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgTradeSetup::IDD, pParent)
{
	m_iXButtonHovering = INVALID_ID;
	m_pImgNextPrev     = NULL;
	m_iCurrPage        = 0;
	m_pDlgMoreTrade    = NULL;

	CString strBrokerType[] = {"ZTZQ","ZXZQ",};
	for (int i = 0 ; i < 2 ; ++i)
	{
		m_mapBrokerIcon.insert(make_pair<CString,CString>(strBrokerType[i], "./image/Broker/" + strBrokerType[i]+".bmp"));
	}
	m_mapTypeIcon.insert(make_pair<int32,CString>(0, "./image/Broker/Future.png"));
	m_mapTypeIcon.insert(make_pair<int32,CString>(1, "./image/Broker/Stock.png"));
}

CDlgTradeSetup::~CDlgTradeSetup()
{
	DEL(m_pImgNextPrev);
	DEL(m_pDlgMoreTrade);
	m_fontStaticText.DeleteObject();
	m_fontCheckText.DeleteObject();
	
	// 清空之前保存
	SaveXml();

	ReleaseChildWindow();
}

void CDlgTradeSetup::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgTradeSetup, CDialog)
	ON_WM_TIMER()
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_MOUSEMOVE()
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave) 
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_MESSAGE(WM_TRADESETTING_MSGBEGIN, OnMsgShowTradeDownload)
	ON_MESSAGE(WM_TRADESETTING_CLOSECHILDWND, OnMsgCloseChildWnd)
	ON_MESSAGE(WM_TRADESETTING_SETDEFAULT, OnMsgSetDefaultChildWnd)
	ON_MESSAGE(WM_TRADESETTING_CHANGEIDLESTATUS, OnMsgChangeIdleStatus)
	ON_MESSAGE(WM_TRADESETTING_HIDEWND, OnMsgHideMainDlg)
END_MESSAGE_MAP()


// CDlgTradeSetup 消息处理程序
BOOL CDlgTradeSetup::OnInitDialog() 
{
	m_pImgNextPrev = Image::FromFile(L"image//BtnNextPrev.png");

	DrawClientButton();
	
	Initialize();
//	m_ProgressDownload.SubclassDlgItem(IDC_PROGRESS_DOWNLOAD, this);

	return TRUE; 
}

BOOL CDlgTradeSetup::OnEraseBkgnd(CDC* pDC)
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

void CDlgTradeSetup::OnMouseMove(UINT nFlags, CPoint point) 
{
	int32 iButton = TButtonHitTest(point);

	if (iButton != m_iXButtonHovering)
	{
		if (INVALID_ID != m_iXButtonHovering)
		{
			m_mapBtn[m_iXButtonHovering].MouseLeave();
			m_iXButtonHovering = INVALID_ID;
		}

		if (INVALID_ID != iButton)
		{	
			m_iXButtonHovering = iButton;

			m_mapBtn[m_iXButtonHovering].MouseHover();
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

LRESULT CDlgTradeSetup::OnMouseLeave(WPARAM wParam, LPARAM lParam)       
{     
	if (INVALID_ID != m_iXButtonHovering)
	{
		m_mapBtn[m_iXButtonHovering].MouseLeave();
		
		m_iXButtonHovering = INVALID_ID;
	}

	return 0;       
} 

void CDlgTradeSetup::OnLButtonDown(UINT nFlags, CPoint point) 
{
	int32 iButton = TButtonHitTest(point);
	if (INVALID_ID != iButton)
	{
		m_mapBtn[iButton].LButtonDown();
	}

}

void CDlgTradeSetup::OnLButtonUp(UINT nFlags, CPoint point) 
{
	int32 iButton = TButtonHitTest(point);
	if (INVALID_ID != iButton)
	{
		m_mapBtn[iButton].LButtonUp();
	}
}

BOOL CDlgTradeSetup::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	CGGTongApp* pApp = DYNAMIC_DOWNCAST(CGGTongApp, AfxGetApp());
	int32 iID = (int32)wParam;
	std::map<int, CNCButton>::iterator it = m_mapBtn.find(iID);
	if (m_mapBtn.end() != it)
	{
		switch (iID)
		{
		case IDC_BTN_PREV:
			{
				m_iCurrPage--;
			}
			break;
		case IDC_BTN_NEXT:
			{
				m_iCurrPage++;
			}
			break;
		default:
			{
			}
			break;
		}

		ChangeTradeBtnStatus();

		InitChildWindow();

		Invalidate();
	}

	return CDialog::OnCommand(wParam, lParam);
}

void CDlgTradeSetup::OnPaint() 
{
	CPaintDC dc(this);

	//ShowChildWindow();
}

void CDlgTradeSetup::AddButton(LPRECT lpRect, Image *pImage, UINT nCount, UINT nID, LPCTSTR lpszCaption)
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
	btnControl.SetTextColor(RGB(102, 102,102), RGB(102, 102,102), RGB(102, 102,102));

	m_mapBtn[nID] = btnControl;
}

int	 CDlgTradeSetup::TButtonHitTest(CPoint point)
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

BOOL CDlgTradeSetup::PreTranslateMessage(MSG* pMsg) 
{
	return BaseDialog::PreTranslateMessage(pMsg);
}
void CDlgTradeSetup::DrawClientButton()
{
	int32 iTitleHeight = 60;
	int32 iWndWidth = 570;
	int32 iWndHeight = 340;
	int32 iTopHeight = 209;
	CRect rcWnd(0, 0, iWndWidth, iWndHeight);

	//MoveWindow(rcWnd);
	CWindowDC dc(this);
	Graphics graphics(dc.GetSafeHdc());

	m_mapBtn.clear();

	CRect rcTmp;
	
	rcTmp.left = rcWnd.right - m_pImgNextPrev->GetWidth() - KiBtnRightSpace;
	rcTmp.top = rcWnd.bottom - m_pImgNextPrev->GetHeight()/4 -KiBtnBottomSpace;
	rcTmp.right = rcTmp.left + m_pImgNextPrev->GetWidth();
	rcTmp.bottom = rcTmp.top + m_pImgNextPrev->GetHeight()/4;
	AddButton(rcTmp, m_pImgNextPrev, 4, IDC_BTN_NEXT, L"下一页");	// 下一页

	rcTmp.left = rcTmp.left - m_pImgNextPrev->GetWidth() - KiBtnRightSpace;
	rcTmp.top = rcWnd.bottom - m_pImgNextPrev->GetHeight()/4 -KiBtnBottomSpace;
	rcTmp.right = rcTmp.left + m_pImgNextPrev->GetWidth();
	rcTmp.bottom = rcTmp.top + m_pImgNextPrev->GetHeight()/4;
	AddButton(rcTmp, m_pImgNextPrev, 4, IDC_BTN_PREV, L"上一页");	// 上一页
}

void CDlgTradeSetup::InitChildWindow()
{
	CRect rt;
	::GetClientRect(m_hWnd, rt);

	int    i = m_iCurrPage * (KiShowTradeNumber-1) ;
	int    iCurrCnt = 0;
	CRect  rcOffsetVH(0, 0, 0, 0);
	
	HideAllChildWnd();

	int32 iCount = m_vecChildWnd.size();

	for (; i < iCount  ; i++)
	{
		if (0 == ( ( iCurrCnt + 1 ) % KiShowTradeNumber) && 0 != iCurrCnt)
		{
			break;
		}
		else if (0 == ( iCurrCnt % (KiShowTradeNumber - 1) ) )
		{
			rcOffsetVH.left +=  KiLeftPos;
			rcOffsetVH.top  +=   KiTopSpace;
		}

 		CDlgTradeSettingData *pChildWnd = m_vecChildWnd.at(i);
		
		rcOffsetVH.right  = rcOffsetVH.left + KiTradeWidth;
		rcOffsetVH.bottom = rcOffsetVH.top  + KiTradeHeight;
		pChildWnd->MoveWindow(rcOffsetVH.left, rcOffsetVH.top, rcOffsetVH.Width(), rcOffsetVH.Height());
		
		if (rcOffsetVH.right + KiTradeWidth > rt.right)
		{
			rcOffsetVH.left =  KiLeftSpace;
			rcOffsetVH.top  =  rcOffsetVH.bottom + KiTopSpace; 
		}
		else
		{
			rcOffsetVH.left =  rcOffsetVH.right + KiLeftSpace;
		}
		// 加载券商logo
		CString StrLogoPath = L""; 
		StrLogoPath.Format(L".\\image\\Broker\\%s",pChildWnd->m_stTradeCfgData.StrLogo);;
		
		pChildWnd->LoadBrokerIcon(StrLogoPath);
		pChildWnd->ShowWindow(SW_SHOW);

		++iCurrCnt;
	}
	if ((iCurrCnt -1) < KiShowTradeNumber)
	{
		if (NULL == m_pDlgMoreTrade)
		{
			m_pDlgMoreTrade = new CDlgTradeSettingData;
			m_pDlgMoreTrade->Create(CDlgTradeSettingData::IDD, this);
		}

		if ( 0 == iCount )
		{
			rcOffsetVH.top   = KiTopSpace;
			rcOffsetVH.left  = KiLeftPos; 
		}
		
		rcOffsetVH.right  = rcOffsetVH.left + KiTradeWidth;
		rcOffsetVH.bottom = rcOffsetVH.top  + KiTradeHeight;
		m_pDlgMoreTrade->MoveWindow(rcOffsetVH.left, rcOffsetVH.top, rcOffsetVH.Width(), rcOffsetVH.Height());

		m_pDlgMoreTrade->ShowWindow(SW_SHOW);
		m_pDlgMoreTrade->m_bIsMoreTrade = TRUE;
	}
}

void CDlgTradeSetup::ReleaseChildWindow()
{
	std::vector<CDlgTradeSettingData *>::iterator it;
	for (it = m_vecChildWnd.begin() ; it != m_vecChildWnd.end() ; ++it)
	{
		delete *it;
		*it = NULL;
	}
	std::vector<CDlgTradeSettingData *>().swap(m_vecChildWnd);
}

void CDlgTradeSetup::Initialize()
{
	CGGTongDoc* pDoc = (CGGTongDoc*)AfxGetDocument();

	CString StrPath = CPathFactory::GetUserTradeCfgFileFullName(pDoc->m_pAbsCenterManager->GetUserName());
	string sPath = _Unicode2MultiChar(StrPath);
	const char* strFile = sPath.c_str();

	if ( 0 != _access ( strFile, 0 ))
	{
		_tcheck_if_mkdir(StrPath.GetBuffer(MAX_PATH));
		StrPath.ReleaseBuffer();
		CString StrDefaultXml  = _T("<?xml version='1.0' encoding='utf-8' ?>\n<XMLDATA version='1.0' app='ggtong' data='config'>\n<trade>\n</trade>\n</XMLDATA>");	
		
		TiXmlDocument* pXmlDoc = new TiXmlDocument(strFile);
		string sDefaultXml     =  _Unicode2MultiChar(StrDefaultXml);
		const char* strDefaultXml = sDefaultXml.c_str();
		pXmlDoc->Parse(strDefaultXml);
		pXmlDoc->SaveFile();
		
		delete pXmlDoc;
	}
	else
	{
		LoadXml();
	}
}

void CDlgTradeSetup::ShowChildWindow()
{	
	// 默认交易程序显示在最前面
	sort(m_vecChildWnd.begin(), m_vecChildWnd.end(), Compare);
	
	InitChildWindow();
	ChangeTradeBtnStatus();
	ResetDownloadFlag();
}

bool32 CDlgTradeSetup::LoadXml()
{
	CGGTongDoc* pDoc = (CGGTongDoc*)AfxGetDocument();
	CString StrPath = CPathFactory::GetUserTradeCfgFileFullName(pDoc->m_pAbsCenterManager->GetUserName());
	
	if ( StrPath.IsEmpty() || !pDoc)
	{
		return false;
	}

	string sPath = _Unicode2MultiChar(StrPath);
	const char* strFilePath = sPath.c_str();

	TiXmlDocument XmlDoc(strFilePath);
	if ( !XmlDoc.LoadFile() )
	{
		return false;
	}

	TiXmlElement* pRootElement = XmlDoc.RootElement();
	if ( NULL == pRootElement )
	{
		return false;
	}

	for (TiXmlElement* pElement = pRootElement->FirstChildElement(); NULL != pElement; pElement = pElement->NextSiblingElement())
	{
		//
		const char* pElementValue = pElement->Value();
		if (NULL == pElementValue)
		{
			continue;
		}

		//
		if (0 == strcmp(pElementValue, "trade"))
		{
			TiXmlElement* pEle = pElement;
			if ( NULL == pEle )
			{
				return false;
			}

			ReleaseChildWindow();
			const char *pszAttr = NULL;
			TiXmlElement *pEleSub = pEle->FirstChildElement("tradeItem");
			while ( NULL != pEleSub )
			{
				CDlgTradeSettingData *pDlg= new CDlgTradeSettingData;
				pDlg->Create(CDlgTradeSettingData::IDD, this);
				
				pszAttr = pEleSub->Attribute("brokerName");
				if ( NULL != pszAttr)
				{
					wstring	str;
					MultiChar2Unicode(CP_UTF8, pszAttr, str);
					pDlg->m_stTradeCfgData.StrBrokerName = str.c_str();
				}

				pszAttr = pEleSub->Attribute("appName");
				if ( NULL != pszAttr)
				{
					wstring	str;
					MultiChar2Unicode(CP_UTF8, pszAttr, str);
					pDlg->m_stTradeCfgData.StrAppName   = str.c_str();
				}

				pszAttr = pEleSub->Attribute("shortcut");
				if ( NULL != pszAttr && strlen(pszAttr) > 0 )
				{
					pDlg->m_stTradeCfgData.StrShortcut  = _A2W(pszAttr);
				}

				pszAttr = pEleSub->Attribute("type");
				if ( NULL != pszAttr && strlen(pszAttr) > 0 )
				{
					pDlg->m_stTradeCfgData.iType = atoi(pszAttr);
				}

				pszAttr = pEleSub->Attribute("run");
				if ( NULL != pszAttr && strlen(pszAttr) > 0 )
				{
					pDlg->m_stTradeCfgData.StrRun = _A2W(pszAttr);
				}

				pszAttr = pEleSub->Attribute("url");
				if ( NULL != pszAttr && strlen(pszAttr) > 0 )
				{
					pDlg->m_stTradeCfgData.StrUrl = _A2W(pszAttr);
				}

				pszAttr = pEleSub->Attribute("logo");
				if ( NULL != pszAttr && strlen(pszAttr) > 0 )
				{
					pDlg->m_stTradeCfgData.StrLogo = _A2W(pszAttr);
				}
				
				pszAttr = pEleSub->Attribute("default");
				if ( NULL != pszAttr && strlen(pszAttr) > 0 )
				{
					pDlg->m_stTradeCfgData.bDefaultTrade = atoi(pszAttr);
				}

				pszAttr = pEleSub->Attribute("isDownload");
				if ( NULL != pszAttr && strlen(pszAttr) > 0 )
				{
					pDlg->m_stTradeCfgData.bDldComplete = atoi(pszAttr);
				}

				pDlg->InitData();
				
				m_vecChildWnd.push_back(pDlg);
				pEleSub = pEleSub->NextSiblingElement("tradeItem");
			}
		}
	}
	
	return true;
}

bool32  CDlgTradeSetup::SaveXml()
{
	CMainFrame *pMainFrm = DYNAMIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
	CGGTongDoc *pDoc = (CGGTongDoc*)AfxGetDocument();
	CString StrPath = CPathFactory::GetUserTradeCfgFileFullName(pDoc->m_pAbsCenterManager->GetUserName());
	if ( StrPath.IsEmpty() || !pDoc || !pMainFrm)
	{
		return false;
	}
	string sPath = _Unicode2MultiChar(StrPath);
	const char* strFilePath = sPath.c_str();

	TiXmlDocument *pXmlDoc = new TiXmlDocument();
	if (NULL == pXmlDoc)
	{
		return false;
	}

	// XML 的版本号
	TiXmlDeclaration *pDeclaration = new TiXmlDeclaration("1.0", "utf-8","yes");
	if (NULL == pDeclaration)
	{
		return false;
	}
	pXmlDoc->LinkEndChild(pDeclaration);

	TiXmlElement *pRootEle = new TiXmlElement("XMLDATA");
	if (NULL == pRootEle)
	{
		return false;
	}
	pXmlDoc->LinkEndChild(pRootEle);

	// SELDATA
	TiXmlElement *pTradeEle = new TiXmlElement(KE_Trade);
	if (NULL == pTradeEle)
	{
		return false;
	}
	int32 iCnt = m_vecChildWnd.size();
	
	// 没有交易程序时需要清空etcConfig.xml中默认交易程序
	if ( 0 == iCnt)
	{
		pMainFrm->SetTradeExePath(NULL);	// 清除不能启动的交易配置
	}

	pRootEle->LinkEndChild(pTradeEle);
	
	for ( int32 i=0; i < iCnt ; i++ )
	{
		T_TradeCfgParam stData = m_vecChildWnd.at(i)->m_stTradeCfgData;

		if (!stData.bDldComplete)
		{
			continue;
		}
		
		TiXmlElement *pTradeItemEle = new TiXmlElement(KE_TradeItem);
		ASSERT(NULL != pTradeItemEle);
		pTradeEle->LinkEndChild(pTradeItemEle);
		
		string strTemp;
		
		// brokerName
		UnicodeToUtf8(stData.StrBrokerName, strTemp);
		pTradeItemEle->SetAttribute(KA_BrokerName, strTemp.c_str());
		// appName
		UnicodeToUtf8(stData.StrAppName, strTemp);
		pTradeItemEle->SetAttribute(KA_AppName, strTemp.c_str());
		// shortcut
		UnicodeToUtf8(stData.StrShortcut, strTemp);
		pTradeItemEle->SetAttribute(KA_Shortcut, strTemp.c_str());
		// type
		pTradeItemEle->SetAttribute(KA_Type, stData.iType);
		// run
		UnicodeToUtf8(stData.StrRun, strTemp);
		pTradeItemEle->SetAttribute(KA_Run,  strTemp.c_str());
		// url
		UnicodeToUtf8(stData.StrUrl, strTemp);
		pTradeItemEle->SetAttribute(KA_Url, strTemp.c_str());
		// logo
		UnicodeToUtf8(stData.StrLogo, strTemp);
		pTradeItemEle->SetAttribute(KA_Logo, strTemp.c_str());
		// default
		pTradeItemEle->SetAttribute(KA_DefaultSel, stData.bDefaultTrade);
		// down complete
		pTradeItemEle->SetAttribute(KA_IsDownload, stData.bDldComplete);
	}
	
	pXmlDoc->SaveFile(strFilePath);

	DEL(pXmlDoc);
	
	return true;
}

LRESULT CDlgTradeSetup::OnMsgShowTradeDownload(WPARAM wParam, LPARAM lParam)
{
	
	CDlgTradeSetting *pDlg = (CDlgTradeSetting*)GetParent();
	pDlg->ShowPage(1);//跳到Download对话框

	return 0;
}

LRESULT CDlgTradeSetup::OnMsgCloseChildWnd(WPARAM wParam, LPARAM lParam)
{
	CGGTongApp *pApp  =  DYNAMIC_DOWNCAST(CGGTongApp, AfxGetApp());
	CDlgTradeSettingData *pDlg   = (CDlgTradeSettingData *)wParam;
	CString    StrAppDir = pDlg->m_stTradeCfgData.StrShortcut;
	CString    StrDir     = L"";
	CString    StrRunPath = L"";
	if (pApp)
	{
		StrDir = CPathFactory::GetPrivateTradePath(pApp->m_pDocument->m_pAbsCenterManager->GetUserName());
	}
	
	StrDir += StrAppDir;
	
	//
	int iRet = 0;

	if (m_stDownloadStatus.pDlg == pDlg)
	{
		iRet = AfxMessageBox(L"正在下载或者解压文件...... \n是否取消?", MB_ICONWARNING|MB_OKCANCEL);
		if (IDOK == iRet)
		{
			ResetDownloadStatus();
		}
		else if(IDCANCEL == iRet)
		{
			return 0;
		}
	}
	else
	{
		iRet = AfxMessageBox(L"交易程序已经下载， \n是否删除该交易程序?", MB_ICONWARNING|MB_OKCANCEL);
		
		if( IDCANCEL == iRet )
		{
			return 0;
		}
	}
	
	// 删除私有目录下已经解压的交易程序
	DeleteDirectory(StrDir);
	
	vector<CDlgTradeSettingData *>::iterator itRet = find( m_vecChildWnd.begin( ), m_vecChildWnd.end( ), pDlg ); //查找要删除的对话框
	if ( itRet != m_vecChildWnd.end( ) )
	{		
		delete *itRet;
		*itRet = NULL;
		m_vecChildWnd.erase(itRet);
	}
	
	if(!m_vecChildWnd.empty())
	{
		if (!m_vecChildWnd.at(0)->m_stTradeCfgData.bDefaultTrade)
		{
			m_vecChildWnd.at(0)->m_stTradeCfgData.bDefaultTrade = true;
			// 把默认程序添加到etc文件中
			StrRunPath.Format(L".\\private\\%s\\trade\\%s\\%s", pApp->m_pDocument->m_pAbsCenterManager->GetUserName(), m_vecChildWnd.at(0)->m_stTradeCfgData.StrShortcut,m_vecChildWnd.at(0)->m_stTradeCfgData.StrRun);
			CEtcXmlConfig::Instance().WriteEtcConfig(KStrTBWndSection, KStrTBWndTradeKey, StrRunPath);
		}
	}
	else
	{
		CEtcXmlConfig::Instance().WriteEtcConfig(KStrTBWndSection, KStrTBWndTradeKey, L"");
	}
	
	ShowChildWindow();
	
	return 0;
}

LRESULT CDlgTradeSetup::OnMsgSetDefaultChildWnd(WPARAM wParam, LPARAM lParam)
{
	CDlgTradeSettingData *pDlg   = (CDlgTradeSettingData *)wParam;

	CGGTongApp *pApp = DYNAMIC_DOWNCAST(CGGTongApp, AfxGetApp());

	std::vector<CDlgTradeSettingData *>::iterator it  = m_vecChildWnd.begin();
	CString StrRunPath = L"";
	
	for (; it != m_vecChildWnd.end(); ++it)
	{
		if (*it == pDlg)
		{
			(*it)->m_stTradeCfgData.bDefaultTrade   = true;
			StrRunPath.Format(L".\\private\\%s\\trade\\%s\\%s", pApp->m_pDocument->m_pAbsCenterManager->GetUserName(), (*it)->m_stTradeCfgData.StrShortcut,(*it)->m_stTradeCfgData.StrRun);

			CEtcXmlConfig::Instance().WriteEtcConfig(KStrTBWndSection, KStrTBWndTradeKey, StrRunPath);
		}
		else
		{
			(*it)->m_stTradeCfgData.bDefaultTrade   = false;
		}
	}
	
	Invalidate();
	
	sort(m_vecChildWnd.begin(), m_vecChildWnd.end(), Compare);
	SaveXml();
	
	return 0;
}

LRESULT CDlgTradeSetup::OnMsgChangeIdleStatus(WPARAM wParam, LPARAM lParam)
{
	CDlgTradeSettingData *pDlg = (CDlgTradeSettingData *)wParam;
	if (m_stDownloadStatus.pDlg == pDlg)
	{
		m_stDownloadStatus.bIdle = (int)lParam == 1;
		m_stDownloadStatus.pDlg  = NULL;
	}

	return 0;
}

LRESULT CDlgTradeSetup::OnMsgHideMainDlg(WPARAM wParam, LPARAM lParam)
{
	CDlgTradeSetting *pMainDlg = (CDlgTradeSetting*)GetParent();
	pMainDlg->EndDialog(IDOK);	//结束主对话框
	
	return 0;
}

void CDlgTradeSetup::AddTradeApp(T_TradeCfgParam &stData)
{
	std::vector<CDlgTradeSettingData *>::iterator it = m_vecChildWnd.begin();
	CGGTongApp *pApp = DYNAMIC_DOWNCAST(CGGTongApp, AfxGetApp());
	CString StrRunPath = L"";
	CDlgTradeSettingData *pDlg = NULL;
	if(!m_vecChildWnd.empty())
	{
		bool32  bFind = false;
		for (; it != m_vecChildWnd.end(); ++it)
		{
			if (0 == stData.StrShortcut.CompareNoCase((*it)->m_stTradeCfgData.StrShortcut))
			{
				bFind = true;
				break;
			}
		}
		if (!bFind)
		{
			// 重置默认交易软件
			ResetDefaultTrade();

			pDlg = new CDlgTradeSettingData;
			pDlg->Create(CDlgTradeSettingData::IDD, this);
			pDlg->m_stTradeCfgData     = stData;
			pDlg->m_stTradeCfgData.bNewAdd  = true;
			pDlg->m_stTradeCfgData.bDefaultTrade = true;
			
			pDlg->InitData();
			
			m_stDownloadStatus.pDlg     = pDlg;
			m_stDownloadStatus.bIdle    = false;

			it = m_vecChildWnd.begin();
			m_vecChildWnd.insert(++it, pDlg);
		}
	}
	else
	{
		pDlg = new CDlgTradeSettingData;
		pDlg->Create(CDlgTradeSettingData::IDD, this);
		
		pDlg->m_stTradeCfgData    = stData;
		pDlg->m_stTradeCfgData.bNewAdd       = true;
		pDlg->m_stTradeCfgData.bDefaultTrade = true;
		pDlg->InitData();
		
		m_stDownloadStatus.pDlg     =   pDlg;
		m_stDownloadStatus.bIdle    =   false;
		
		m_vecChildWnd.push_back(pDlg);
	}
	
	m_iCurrPage = 0;

	if (NULL != pDlg)
	{
		//	把默认交易软件添加到etc文件中
		StrRunPath.Format(L".\\private\\%s\\trade\\%s\\%s", pApp->m_pDocument->m_pAbsCenterManager->GetUserName(),pDlg->m_stTradeCfgData.StrShortcut,pDlg->m_stTradeCfgData.StrRun);
		CEtcXmlConfig::Instance().WriteEtcConfig(KStrTBWndSection, KStrTBWndTradeKey, StrRunPath);
	}
}

void CDlgTradeSetup::ResetDownloadFlag()
{
	std::vector<CDlgTradeSettingData *>::iterator it = m_vecChildWnd.begin();

	for(; it != m_vecChildWnd.end(); ++it)
	{
		(*it)->m_stTradeCfgData.bNewAdd = false;
	}
}

void CDlgTradeSetup::ResetDefaultTrade()
{
	std::vector<CDlgTradeSettingData *>::iterator it = m_vecChildWnd.begin();

	for(; it != m_vecChildWnd.end(); ++it)
	{
		(*it)->m_stTradeCfgData.bDefaultTrade = false;
	}
}

void CDlgTradeSetup::HideAllChildWnd()
{
	std::vector<CDlgTradeSettingData *>::iterator it = m_vecChildWnd.begin();

	for(; it != m_vecChildWnd.end(); ++it)
	{
		(*it)->ShowWindow(SW_HIDE);
	}
}

bool32 CDlgTradeSetup::GetDownloadStatusFlag()
{
	return m_stDownloadStatus.bIdle;
}

void CDlgTradeSetup::ResetDownloadStatus()
{
	m_stDownloadStatus.bIdle = true;
	m_stDownloadStatus.pDlg  = NULL;
}

void CDlgTradeSetup::ChangeTradeBtnStatus()
{
	int32 iCount = m_vecChildWnd.size();
	int32 iPageSize = (iCount%5) ? (iCount/5 + 1):(iCount/5);

	if (0 == m_iCurrPage)
	{
		m_mapBtn[IDC_BTN_PREV].EnableButton(FALSE);
	}
	else
	{
		if (!m_mapBtn[IDC_BTN_PREV].IsEnable())
		{
			m_mapBtn[IDC_BTN_PREV].EnableButton(TRUE);
		}
	}

	if ( (iPageSize - 1) == m_iCurrPage)
	{
		m_mapBtn[IDC_BTN_NEXT].EnableButton(FALSE);
	}
	else
	{
		if (!m_mapBtn[IDC_BTN_NEXT].IsEnable())
		{
			m_mapBtn[IDC_BTN_NEXT].EnableButton(TRUE);
		}
	}
}