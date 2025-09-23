#include "stdafx.h"
//#include "resource.h"
#include "MPIChildFrame.h"
#include "IoViewManager.h"
#include "MerchManager.h"
//#include "ViewData.h"
#include "facescheme.h"
#include "ShareFun.h"
#include "IoViewNews.h"
#include "sha1.h"
#include "coding.h"
#include "CCodeFile.h"
#include "FontFactory.h"
#include "DlgTodayComment.h"
#include "PathFactory.h"

#define	CEFWND_NEWS_ID		12346
#define INVALID_ID -1
#define BTN_HEIGHT		25
#define BTN_MORE_WIDTH	50

#pragma optimize("g", off)	// necessary in VC 6.0 to suppress (release mode)
//    "fatal error C1001: INTERNAL COMPILER ERROR"

///////////////////////////////////
static const char *KStrElementAttrLocationUrl		= "LocationUrl";
static const char *KStrElementAttrCustomUrl			= "CustomUrl";
static const char *KStrElementAttrMoreUrl			= "MoreUrl";
static const char *KStrElementAttrMoreType			= "MoreShowType";
static const char *KStrElementAutoRefresh			= "AutoRefresh";
static const char *KStrElementIsWebNews				= "IsWebNews";
static const char *KStrElementShowBtnName			="ShowBtnName";
static const char *KStrElementNameBkClr				="NameBkClr";
static const char *KStrElementNameTextClr			="NameTextClr";
static const char *KStrElementTreeId			    ="TreeId";


//
static const UINT KuiTimerIDRefresh					= 1000;
static const UINT KuiTimerPeriodRefresh				= 1000*60;			// 一分钟刷新一次
 
IMPLEMENT_DYNCREATE(CIoViewNews, CIoViewBase)

///////////////////////////////////////////////////////////////////////////////
// message map
BEGIN_MESSAGE_MAP(CIoViewNews, CIoViewBase)
//{{AFX_MSG_MAP(CIoViewNews)
ON_WM_PAINT()
ON_WM_ERASEBKGND()
ON_WM_LBUTTONDOWN()
ON_WM_LBUTTONUP()
ON_WM_MOUSEMOVE()
ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave) 
ON_WM_CREATE()
ON_WM_SIZE()
ON_WM_TIMER()
//}}AFX_MSG_MAP
ON_WM_GETMINMAXINFO()
END_MESSAGE_MAP()

///////////////////////////////////////////////////////////////////////////////
// 
CIoViewNews::CIoViewNews():CIoViewBase()
{
	m_bInitialized = false;
	m_CustomUrl.Parser(L"http://www.baidu.com");
	m_StrMoreUrl = L"";
	m_iTimerPeriod = -1;
	m_isWebNews = 0;
	m_strLobbyUrl = L"";
	m_StrShowName = L"";
	m_iMoreBtnHovering = INVALID_ID;
	m_iWebYPos = 0;

	m_clrNameBk = RGB(238,69,2);
	m_clrNameText = RGB(201,208,214);
	m_bFromXml = true;
	m_eMoreShowType = EMST_WSP;
	m_StrTreeId = L"";

}

///////////////////////////////////////////////////////////////////////////////
// 
CIoViewNews::~CIoViewNews()
{
	CGGTongApp* pApp = (CGGTongApp*)AfxGetApp();
	if ( NULL != pApp && !pApp->m_bBindHotkey )
	{
		pApp->m_bBindHotkey = true;
	}
}

///////////////////////////////////////////////////////////////////////////////
// OnPaint
void CIoViewNews::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	if ( !IsWindowVisible() )
 	{
 		return;
 	}

	if (!m_StrShowName.IsEmpty())
	{
		CDC memDC;
		memDC.CreateCompatibleDC(&dc);
		CBitmap bmp;
		CRect rcWindow;
		GetClientRect(&rcWindow);
		rcWindow.bottom = rcWindow.top + BTN_HEIGHT;

		bmp.CreateCompatibleBitmap(&dc, rcWindow.Width(), rcWindow.Height());
		memDC.SelectObject(&bmp);
		memDC.FillSolidRect(rcWindow, RGB(44,50,55));
		memDC.SetBkMode(TRANSPARENT);
		Gdiplus::Graphics graphics(memDC.GetSafeHdc());

		CRect rtBtnName(0, 0, 70, BTN_HEIGHT-1);
		m_BtnName.SetRect(&rtBtnName);

	//	Pen pen(Color(50,50,50), 2); // 绘制边框
	//	graphics.DrawRectangle(&pen, rcWindow.left, rcWindow.top, rcWindow.Width(), rcWindow.Height()-1);

		CRect rtBtnMore(0, 0, 0, BTN_HEIGHT-1);
		rtBtnMore.top += 3;
		rtBtnMore.bottom -=3;
		rtBtnMore.left = rcWindow.right - BTN_MORE_WIDTH-2;
		rtBtnMore.right = rcWindow.right-2;
		m_BtnMore.SetRect(&rtBtnMore);

		m_BtnName.DrawButton(&graphics);
		m_BtnMore.DrawButton(&graphics);

		dc.BitBlt(0, 0, rcWindow.Width(), rcWindow.Height(), &memDC, 0, 0, SRCCOPY);
		dc.SelectClipRgn(NULL);
		memDC.DeleteDC();
		bmp.DeleteObject();
	}

	if ( GetParentGGTongViewDragFlag() || m_bDelByBiSplitTrack )
	{		
		LockRedraw();
		return;
	}

	UnLockRedraw();
}

BOOL CIoViewNews::OnEraseBkgnd(CDC* pDC)
{
	return true;
}

///////////////////////////////////////////////////////////////////////////////
// OnLButtonDown
void CIoViewNews::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (!m_StrShowName.IsEmpty())
	{
		int32 iButton = 0;
		iButton = TButtonHitTest(point);

		if (INVALID_ID != iButton)
		{
			if (iButton == m_BtnMore.GetControlId())
			{
				m_BtnMore.LButtonDown();
			}
		}
	}

	CStatic::OnLButtonDown(nFlags, point);
}

///////////////////////////////////////////////////////////////////////////////
// OnLButtonUp
void CIoViewNews::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (!m_StrShowName.IsEmpty())
	{
		int32 iButton = 0;
		iButton = TButtonHitTest(point);

		if (INVALID_ID != iButton)
		{
			if (iButton == m_BtnMore.GetControlId())
			{
				m_BtnMore.LButtonUp();
			}
		}
	}

	CStatic::OnLButtonUp(nFlags, point);
}

///////////////////////////////////////////////////////////////////////////////
// OnMouseMove
void CIoViewNews::OnMouseMove(UINT nFlags, CPoint point)
{	
	if (!m_StrShowName.IsEmpty())
	{
		int32 iButton = TButtonHitTest(point);
		if (INVALID_ID != iButton)
		{	
			if (iButton == m_BtnMore.GetControlId())
			{
				m_BtnMore.MouseHover();
				m_iMoreBtnHovering = iButton;
			}
			else
			{
				m_iMoreBtnHovering = INVALID_ID;
			}
		}
		else
		{
			m_BtnMore.MouseLeave();
		}
	}

	// 响应 WM_MOUSELEAVE消息
	TRACKMOUSEEVENT csTME;
	csTME.cbSize	= sizeof (csTME);
	csTME.dwFlags	= TME_LEAVE;
	csTME.hwndTrack = m_hWnd ;
	::_TrackMouseEvent (&csTME);

	CStatic::OnMouseMove(nFlags, point);
} 

LRESULT CIoViewNews::OnMouseLeave(WPARAM wParam, LPARAM lParam)    
{
	if (!m_StrShowName.IsEmpty())
	{
		if (INVALID_ID != m_iMoreBtnHovering)
		{
			if (m_iMoreBtnHovering == m_BtnMore.GetControlId())
			{
				m_BtnMore.MouseLeave();
			}

			m_iMoreBtnHovering = INVALID_ID;
		}
	}
	return 0;
}

BOOL CIoViewNews::OnCommand( WPARAM wParam, LPARAM lParam )
{
	int32 iID = (int32)wParam;
	if (BTN_MORE == iID)
	{
		if(m_StrMoreUrl == L"")
		{
			return FALSE;
		}

		switch (m_eMoreShowType)
		{
		case EMST_WSP:
			{
				ShowMoreInWsp();
				break;
			}
		case EMST_DLG:
			{
				ShowMoreInDlg();
				break;
			}
		case EMST_WEB:
			{
				ShowMoreInExternal();
				break;
			}
		default:
			{
				ShowMoreInWsp();
			}
		}
	}

	return CIoViewBase::OnCommand(wParam, lParam);
}

int CIoViewNews::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	CIoViewBase::OnCreate(lpCreateStruct);

	if (m_iTimerPeriod > 0)
	{
		SetTimer(KuiTimerIDRefresh, 1000 * m_iTimerPeriod, NULL);
	}
	
	return 0;
}

//////////////////////////////////////////////////////////////////////////
BOOL CIoViewNews::DestroyWindow()
{
	// KillTimer(KuiTimerIDRefresh);
	return CIoViewBase::DestroyWindow();
}

void CIoViewNews::OnTimer(UINT nIDEvent)
{
	// 定时刷新
	if ( KuiTimerIDRefresh == nIDEvent )
	{
		try
		{
			if ( GetParentGGTongViewDragFlag() || m_bDelByBiSplitTrack )
			{
				//NULL;
			}
			else
			{
				m_wndCef.ReLoad();
			}			
		}
		catch( CException* e)
		{
			e->GetRuntimeClass();
		}
	}
	CIoViewBase::OnTimer(nIDEvent);
}

void CIoViewNews::OnSize(UINT nType, int cx, int cy) 
{
	CIoViewBase::OnSize(nType, cx, cy);
	CGGTongApp* pApp = (CGGTongApp*)AfxGetApp();
	if(cx != 0 && cy != 0)
	{
		if(m_wndCef.GetSafeHwnd() == NULL)
		{

			if(m_isWebNews == 1)
			{
				CWnd *pMain = AfxGetMainWnd();
				CRect rect;
				pMain->GetClientRect(&rect);
				

				if(cx == rect.Width())	// 全屏
				{	
					m_CustomUrl.SetPath(L"/news/newsMain.html");
					if(ID_SPECIAL_INFO_JP == pApp->m_pConfigInfo->m_nSelectNewsID)
					{
						CString StrSSJPTabId = pApp->m_pConfigInfo->m_StrSSJPTabId;
						m_CustomUrl.SetQueryValue(L"treeid", StrSSJPTabId);
					}
					else if (ID_SPECIAL_INFO_GJS == pApp->m_pConfigInfo->m_nSelectNewsID)
					{
						CString StrJPZXTabId = pApp->m_pConfigInfo->m_StrJPZXTabId;
						m_CustomUrl.SetQueryValue(L"treeid", StrJPZXTabId);
					}
				}

			}

			m_wndCef.CreateWndCef(CEFWND_NEWS_ID,this, CRect(0, m_iWebYPos, cx, cy),m_CustomUrl.GetUrl());
		}
		else
		{
			//屏蔽右上角广告滚动条
			if (3 == m_isWebNews)
				m_wndCef.MoveWindow(0, 0, cx + 18, cy + 18);
			else
				m_wndCef.MoveWindow(0, m_iWebYPos, cx, cy-m_iWebYPos);
		}
	}
}

BOOL CIoViewNews::TestKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if ( VK_F7 == nChar )
	{
		CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
		pMainFrame->OnProcessF7();

		return TRUE;
	}

	return FALSE;
}

// 通知视图改变关注的商品
void CIoViewNews::OnVDataMerchChanged(int32 iMarketId, const CString &StrMerchCode, IN CMerch *pMerch)
{
	if (m_pMerchXml == pMerch)
		return;

	CGGTongApp* pApp = (CGGTongApp*)AfxGetApp();
	CGGTongDoc *pDoc = pApp->m_pDocument;
	CMainFrame *pMainFrm = DYNAMIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
	
	if (NULL == pApp || NULL == pDoc || NULL == pMainFrm)
	{
		return;
	}
	
	// 修改当前查看的商品
	m_pMerchXml					= pMerch;
	m_MerchXml.m_iMarketId		= pMerch->m_MerchInfo.m_iMarketId;
	m_MerchXml.m_StrMerchCode	= pMerch->m_MerchInfo.m_StrMerchCode;

	// 当前商品不为空的时候
	if (NULL != pMerch)
	{
		// 设置关注的商品信息
		m_aSmartAttendMerchs.RemoveAll();
		
		//--- wangyongxue 2017/03/13 增加判断防止IoviewNews抢焦点导致其他视图无法用鼠标滚轮切换商品
		CMPIChildFrame *pActiveFrame = (CMPIChildFrame *)pMainFrm->MDIGetActive();
		CMPIChildFrame * pParentFrame = DYNAMIC_DOWNCAST(CMPIChildFrame, GetParentFrame());
		
		if (NULL == pActiveFrame || NULL == pParentFrame 
			|| pParentFrame != pActiveFrame)
		{
			return;
		}
		
		if (m_bInitialized &&  6 == m_isWebNews)
		{
			m_CustomUrl.Parser(GetStockNewsUrl());
			RefreshWeb(m_CustomUrl.GetUrl());
			m_wndCef.SetWebFocusStatus(false);
		}
		
	}
	else
	{
		// zhangbo 20090824 #待补充， 当心商品不存在时，清空当前显示数据
		//...
	}
}

//
void CIoViewNews::OnVDataForceUpdate()
{
}

bool32 CIoViewNews::FromXml(TiXmlElement * pElement)
{
	if (NULL == pElement)
		return false;

	CString StrMerchCode = L"";
	int32   iMarketId = 0;

	// 判读是不是IoView的节点
	const char *pcValue = pElement->Value();
	if (NULL == pcValue || strcmp(GetXmlElementValue(), pcValue) != 0)
		return false;

	// 判断是不是描述自己这个业务视图的节点
	const char *pcAttrValue = pElement->Attribute(GetXmlElementAttrIoViewType());
	if (NULL == pcAttrValue || CIoViewManager::GetIoViewString(this).Compare(CString(pcAttrValue)) != 0)	// 不是描述自己的业务节点
		return false;

	pcAttrValue = pElement->Attribute(KStrElementShowBtnName);
	if (NULL != pcAttrValue)
	{
		wchar_t awcShowName[100];
		memset(awcShowName, 0, sizeof(awcShowName));

		MultiCharCoding2Unicode(EMCCUtf8, pcAttrValue, strlen(pcAttrValue), awcShowName, sizeof(awcShowName) / sizeof(wchar_t));
		m_StrShowName = awcShowName;
	}

	// 读取本业务视图特有的内容
	CString StrLocationUrl;
	pcAttrValue = pElement->Attribute(KStrElementAttrLocationUrl);
	if (NULL != pcAttrValue)
	{
		wchar_t awcLocationUrl[100];
		memset(awcLocationUrl, 0, sizeof(awcLocationUrl));

		MultiCharCoding2Unicode(EMCCUtf8, pcAttrValue, strlen(pcAttrValue), awcLocationUrl, sizeof(awcLocationUrl) / sizeof(wchar_t));
		StrLocationUrl = awcLocationUrl;
	}
	// 读取是否是网页新闻资讯的参数
	pcAttrValue = pElement->Attribute(KStrElementIsWebNews);
	if (NULL != pcAttrValue)
	{
		m_isWebNews = atoi(pcAttrValue);
	}

	pcAttrValue = pElement->Attribute(KStrElementNameBkClr);
	if ((NULL!=pcAttrValue) && strlen(pcAttrValue) > 0)
	{
		m_clrNameBk = StrToColorRef(pcAttrValue);
	}

	pcAttrValue = pElement->Attribute(KStrElementNameTextClr);
	if ((NULL!=pcAttrValue) && strlen(pcAttrValue) > 0)
	{
		m_clrNameText = StrToColorRef(pcAttrValue);
	}
	
	//
	CString strTmpUrl = _T("");
	pcAttrValue = pElement->Attribute(KStrElementAttrCustomUrl);
	if (NULL != pcAttrValue)
	{
		m_CustomUrl.Parser(_A2W(pcAttrValue));
	}

	pcAttrValue = pElement->Attribute(KStrElementAttrMoreUrl);
	if (NULL != pcAttrValue)
	{
		wchar_t awcMoreUrl[512];
		memset(awcMoreUrl, 0, sizeof(awcMoreUrl));

		MultiCharCoding2Unicode(EMCCUtf8, pcAttrValue, strlen(pcAttrValue), awcMoreUrl, sizeof(awcMoreUrl) / sizeof(wchar_t));
		m_StrMoreUrl = awcMoreUrl;
	}

	pcAttrValue = pElement->Attribute(KStrElementTreeId);
	if (NULL != pcAttrValue)
	{
		wchar_t awcTreeId[512];
		memset(awcTreeId, 0, sizeof(awcTreeId));

		MultiCharCoding2Unicode(EMCCUtf8, pcAttrValue, strlen(pcAttrValue), awcTreeId, sizeof(awcTreeId) / sizeof(wchar_t));
		m_StrTreeId = awcTreeId;
	}

	pcAttrValue = pElement->Attribute(KStrElementAttrMoreType);
	if ((NULL!=pcAttrValue) && (strlen(pcAttrValue)>0))
	{
		m_eMoreShowType = E_MoreShowType(atoi(pcAttrValue));
	}

	//
	pcAttrValue = pElement->Attribute(KStrElementAutoRefresh);
	if (NULL != pcAttrValue)
	{
		m_iTimerPeriod = atoi(pcAttrValue);
	}
	// 读取默认商品信息
	pcAttrValue = pElement->Attribute(GetXmlElementAttrMarketId());
	if (NULL != pcAttrValue)
	{
		iMarketId = atoi(pcAttrValue);
	}

	pcAttrValue = pElement->Attribute(GetXmlElementAttrMerchCode());
	if (NULL != pcAttrValue)
	{
		StrMerchCode = _A2W(pcAttrValue);
	}

	CMerch *pMerchFound = NULL;
	
	if ( !StrMerchCode.IsEmpty() )
	{
		if (!m_pAbsCenterManager->GetMerchManager().FindMerch(StrMerchCode, iMarketId, pMerchFound))
		{
			pMerchFound = NULL;
		}
		if ( NULL == pMerchFound )
		{
			// 后面的属性比较重要，找到一个商品给与初始化吧
			CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
			pMerchFound = pFrame->GetIoViewGroupMerch(GetIoViewGroupId());
			if ( NULL == pMerchFound && m_pAbsCenterManager->GetMerchManager().m_BreedListPtr.GetSize() > 0 )
			{
				CBreed *pBreed = m_pAbsCenterManager->GetMerchManager().m_BreedListPtr[0];
				if ( NULL != pBreed && pBreed->m_MarketListPtr.GetSize() > 0 )
				{
					CMarket *pMarket = pBreed->m_MarketListPtr[0];
					if ( NULL != pMarket && pMarket->m_MerchsPtr.GetSize() > 0 )
					{
						pMerchFound = pMarket->m_MerchsPtr[0];
					}
				}
			}
		}
	}
	
	m_pMerchXml = pMerchFound;

	//
	if (m_CustomUrl.GetUrl().GetLength() > 0)
	{
		m_bInitialized = true;
	}

	if ((m_iTimerPeriod > 0) && m_bInitialized)
	{
		SetTimer(KuiTimerIDRefresh, 1000 * m_iTimerPeriod, NULL);
	}

	InitButtons();
	OpenWebNewsCenter();

	return true;
}

CString CIoViewNews::ToXml()
{
	CString StrThis;

	CString StrMarketId;
	StrMarketId.Format(L"%d", m_MerchXml.m_iMarketId);

	// 保存相对的二级地址
	CString StrUrlSave	 = L"";
	CString StrLoacalUrl = L"";//m_WebBrowser2.GetLocationURL();	
	int32	iLenRoot	 = 0;//m_StrServerNewsAddress.GetLength();
	int32   iPos		 = StrLoacalUrl.Find('/',iLenRoot);
	
	if ( -1 != iPos )
	{
		// 后面有多级节点
		StrUrlSave	 = StrLoacalUrl.Mid(iLenRoot, iPos - iLenRoot + 1);
	}
	else
	{
		// 没有,保存根节点后面的部分就行了
		int32 iLenLocal = StrLoacalUrl.GetLength();	
		if ( iLenLocal >= iLenRoot )
		{
			StrUrlSave	= StrLoacalUrl.Mid(iLenRoot, iLenLocal - iLenRoot);
		}
	}
		
	if( StrUrlSave.GetLength() <= 0 )
	{
		StrUrlSave = L"";
	}

	CString strCustomUrl;
	if(0 == m_isWebNews)
	{
		strCustomUrl = m_CustomUrl.GetUrl();
		strCustomUrl.Replace(_T("&"), _T("&amp;")); // 把“&”符合，替换成“&amp;”，不然保存到XML时，&符合会被过滤掉
	}

	m_StrMoreUrl.Replace(_T("&"), _T("&amp;")); 

	CString StrNameBkClr, StrNameTextClr;	
	StrNameBkClr = ColorRefToStr(m_clrNameBk);
	StrNameTextClr = ColorRefToStr(m_clrNameText);

	// 
	StrThis.Format(L"<%s %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%d\" %s=\"%d\">\n", 
		CString(GetXmlElementValue()).GetBuffer(), 
		CString(GetXmlElementAttrIoViewType()).GetBuffer(),
		CIoViewManager::GetIoViewString(this).GetBuffer(),
		CString(KStrElementAttrLocationUrl).GetBuffer(), 
		StrUrlSave.GetBuffer(),
		CString(KStrElementAttrCustomUrl).GetBuffer(),
		strCustomUrl.GetBuffer(),
		CString(KStrElementAttrMoreUrl).GetBuffer(),
		m_StrMoreUrl.GetBuffer(),
		CString(KStrElementTreeId).GetBuffer(),
		m_StrTreeId.GetBuffer(),
		CString(KStrElementShowBtnName).GetBuffer(),
		m_StrShowName.GetBuffer(),
		CString(KStrElementNameBkClr).GetBuffer(),
		StrNameBkClr.GetBuffer(),
		CString(KStrElementNameTextClr).GetBuffer(),
		StrNameTextClr.GetBuffer(),
		CString(GetXmlElementAttrShowTabName()).GetBuffer(),
		m_StrTabShowName.GetBuffer(),
		CString(KStrElementAttrMoreType).GetBuffer(),
		m_eMoreShowType,
		CString(KStrElementIsWebNews).GetBuffer(),
		m_isWebNews);
	//
	StrThis += L"</";
	StrThis += GetXmlElementValue();
	StrThis += L">\n";

	return StrThis;
}

CString CIoViewNews::GetDefaultXML()
{
	CString StrThis;

	// 
	StrThis.Format(L"<%s %s=\"%s\" %s=\"%s\" %s=\"%s\" >\n", 
		CString(GetXmlElementValue()).GetBuffer(), 
		CString(GetXmlElementAttrIoViewType()).GetBuffer(), 
		CIoViewManager::GetIoViewString(this).GetBuffer(),
		CString(GetXmlElementAttrMerchCode()).GetBuffer(), 
		L"",
		CString(GetXmlElementAttrMarketId()).GetBuffer(), 
		L"-1");

	//
	StrThis += L"</";
	StrThis += GetXmlElementValue();
	StrThis += L">\n";

	return StrThis;
}

void CIoViewNews::OnIoViewActive()
{
	return;
	CGGTongApp* pApp = (CGGTongApp*)AfxGetApp();
	if ( NULL != pApp )
	{
		pApp->m_bBindHotkey = false;
	}

	RequestLastIgnoredReqData();

	SetChildFrameTitle();
}

void CIoViewNews::OnIoViewDeactive()
{
	return;
	CGGTongApp* pApp = (CGGTongApp*)AfxGetApp();
	if ( NULL != pApp && !pApp->m_bBindHotkey )
	{
		pApp->m_bBindHotkey = true;
	}
}

void CIoViewNews::ChangeWebStatus(bool32 bActive)
{
	// 在有的电脑容易崩溃，暂时注释掉--chenfj
	//if (m_wndCef.GetSafeHwnd())
	//{
	//	m_wndCef.ReLoadCfmUrl(bActive);
	//}
}

void CIoViewNews::LockRedraw()
{
	if ( m_bLockRedraw )
	{
		return;
	}

	m_bLockRedraw = true;
	::SendMessage(GetSafeHwnd(), WM_SETREDRAW, (WPARAM)(FALSE), 0L);
}




UrlParser CIoViewNews::SetCustomUrlQueryValue(CString strKey, CString strValue)
{
	m_CustomUrl.SetQueryValue(strKey, strValue);
	return m_CustomUrl;
}


UrlParser CIoViewNews::GetCustomUrl()
{
	return m_CustomUrl;
}



void CIoViewNews::UnLockRedraw()
{	
	if ( !m_bLockRedraw )
	{
		return;
	}

	m_bLockRedraw = false;
	::SendMessage(GetSafeHwnd(), WM_SETREDRAW, (WPARAM)(TRUE), 0L);	
}

void CIoViewNews::SetChildFrameTitle()
{
	CString StrTitle = CIoViewManager::FindIoViewObjectByIoViewPtr(this)->m_StrLongName;

	if ( NULL != m_pMerchXml )
	{			
		StrTitle += L" ";
		StrTitle += m_pMerchXml->m_MerchInfo.m_StrMerchCnName;
	}
	
	CMPIChildFrame * pParentFrame = DYNAMIC_DOWNCAST(CMPIChildFrame, GetParentFrame());
	if ( NULL != pParentFrame )
	{
		pParentFrame->SetChildFrameTitle(StrTitle);
	}
}

void CIoViewNews::OpenWebNewsCenter()
{
	CGGTongApp* pApp = (CGGTongApp*)AfxGetApp();
	CGGTongDoc *pDoc = pApp->m_pDocument;
	if (NULL == pApp || NULL == pDoc)
	{
		return;
	}
    if(1 == m_isWebNews)	//网页资讯中心
    {
		m_CustomUrl.Parser(GetWebNewsUrl());
    }
    else if(2 == m_isWebNews)	//内嵌的视频直播
    {
		m_CustomUrl.Parser(L"alias://videoLive"); 

        if (!pApp->m_bCustom)
        {
            CGGTongDoc *pDoc = pApp->m_pDocument;
            m_CustomUrl.SetQueryValue(L"uname",pDoc->m_pAbsCenterManager->GetUserName());

            string strCode;
            CString tmpPwd = pDoc->m_pAbsCenterManager->GetUserPwd();
            UnicodeToUtf8(tmpPwd, strCode);

            unsigned char dst[1024];
            memset(dst, 0, sizeof(dst));
            unsigned char src[1024];
            memset(src, 0, sizeof(src));
            memcpy(src, strCode.c_str(), strCode.size());
            int32 dlen = sizeof(dst);
            int32 slen = strlen((const char *)src);
            CCodeFile::base64_encode(dst, &dlen, src, slen);

            USES_CONVERSION;
			m_CustomUrl.SetQueryValue(L"pwd",_A2W((LPCSTR)dst));
        }
    }
	else if(3 == m_isWebNews)	//广告
	{
		CString StrAdvsCode = pApp->m_pConfigInfo->m_StrTopRightAdvsCode;
		StrAdvsCode = pApp->m_pConfigInfo->GetAdvsCenterUrlByCode(StrAdvsCode,EAUTPit);
		m_CustomUrl.Parser(StrAdvsCode); 
	}
	else if(4 == m_isWebNews)	//文字直播
	{
		m_CustomUrl.Parser(L"alias://textLive");

		CMainFrame* pMain = DYNAMIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
		
		// 参数有效，要带在url后面
		if ((pMain->m_stWebOpenCfm.iID!=-1) && (!pMain->m_stWebOpenCfm.StrFlag.IsEmpty()))
		{
			CString StrID = L"0";
			StrID.Format(L"%d", pMain->m_stWebOpenCfm.iID);

		
			m_CustomUrl.SetQueryValue(L"roomid", StrID);
			m_CustomUrl.SetQueryValue(L"question", pMain->m_stWebOpenCfm.StrFlag);

			pMain->m_stWebOpenCfm.StrCfmName = L"";
			pMain->m_stWebOpenCfm.iID = -1;
			pMain->m_stWebOpenCfm.StrFlag = L"";
		}
	}
	else if(5 == m_isWebNews)	//文字直播(带参数，显示更多)
	{
		m_CustomUrl.Parser(L"alias://textLive");
		m_CustomUrl.SetQueryValue(L"room", L"1");
		m_CustomUrl.SetQueryValue(L"type", L"big");
	}
	else if (6 == m_isWebNews)
	{	
		m_CustomUrl.Parser(GetStockNewsUrl());
	}
	else if (7 == m_isWebNews)
	{
		m_CustomUrl.Parser(GetWebNewsListUrl());
	}
	else if (8 == m_isWebNews)	// 量化操盘
	{
		m_CustomUrl.Parser(GetQuantifyTraderUrl());
	}
	else if(9 == m_isWebNews)
	{
		m_CustomUrl.Parser(GetWebNewsUrl());
	}
	else if(10 == m_isWebNews)
	{
		m_CustomUrl.Parser(GetWebNewsUrlByTreeId(m_StrTreeId));
	}
	//替换账号密码
	CString strValue = m_CustomUrl.GetQueryValue(L"username");
	if (!strValue.IsEmpty())
	{
		strValue = pDoc->m_pAbsCenterManager->GetUserName();
		m_CustomUrl.SetQueryValue(L"username",strValue);
	}
}


CString CIoViewNews::GetOrgCode(LPCTSTR urlParam /*= NULL*/,bool BSha1/* = true*/)
{
	UrlParser uParser;
	uParser.SetQuery(urlParam);

	CGGTongDoc *pDoc =DYNAMIC_DOWNCAST(CGGTongDoc, AfxGetDocument());

	//设置 orgcode
	CString strOrgCode = uParser.GetQueryValue(L"orgcode");
	if (strOrgCode.IsEmpty() && NULL != pDoc)
	{
		strOrgCode = pDoc->m_pAutherManager->GetInterface()->GetServiceDispose()->GetOrgCode();
		uParser.SetQueryValue(L"orgcode",strOrgCode);
	}

	USES_CONVERSION;
	if (!strOrgCode.IsEmpty() && BSha1)
	{
		string sOrgCode;
		UnicodeToUtf8(strOrgCode, sOrgCode);
		char* sha1Code = sha1_hash(sOrgCode.c_str());
		uParser.SetQueryValue(L"orgcode",_A2W(sha1Code));
	}

	//设置 webcode
	CString strPlatCode = uParser.GetQueryValue(L"webcode");
	if (strOrgCode.IsEmpty()  && NULL != pDoc)
	{
		strPlatCode = pDoc->m_pAutherManager->GetInterface()->GetServiceDispose()->GetCodePlatForm();
		uParser.SetQueryValue(L"webcode",strPlatCode);
	}

	return uParser.GetQuery();
}

CString	CIoViewNews::GetQuantifyTraderUrl()
{
	UrlParser uParser(L"alias://quantifytrader"); 

	CString strValue = uParser.GetQuery();
	strValue = GetOrgCode(strValue,false);
	uParser.SetQuery(strValue);

	return uParser.GetUrl();
}

CString CIoViewNews::GetWebNewsUrl(LPCTSTR urlAddParam/* = NULL*/)
{
	UrlParser uParser(L"alias://webnews"); 
	
	CString strValue = uParser.GetQuery();
	strValue = GetOrgCode(strValue);
	uParser.SetQuery(strValue);

	return uParser.GetUrl();
}

CString CIoViewNews::GetWebNewsUrlByTreeId( CString StrTreeId )
{
	UrlParser uParser(L"alias://webnewsbranch"); 
	CString strValue = uParser.GetQuery();
	strValue = GetOrgCode(strValue);
	uParser.SetQuery(strValue);

    uParser.SetQueryValue(L"treeID",StrTreeId);
	CString str = uParser.GetUrl();
    return uParser.GetUrl();
}

CString	CIoViewNews::GetWebNewsListUrl()
{
	UrlParser uParser(GetWebNewsUrl());  
	uParser.SetPath(L"/PCtgweb/newslist/index.html");
	
	return uParser.GetUrl();
}

CString CIoViewNews::GetStockNewsUrl()
{
	UrlParser uParser(L"alias://stocknews");

	CString strValue = uParser.GetQuery();
	strValue = GetOrgCode(strValue);
	uParser.SetQuery(strValue);

	if (NULL != m_pMerchXml)
	{
		CString StrMarketId = L"";
		string sStockName = "";
		StrMarketId.Format(L"%d", m_pMerchXml->m_MerchInfo.m_iMarketId);

		uParser.SetQueryValue(L"market", StrMarketId);
		uParser.SetQueryValue(L"stockCode", m_pMerchXml->m_MerchInfo.m_StrMerchCode);
		uParser.SetQueryValue(L"stockName", m_pMerchXml->m_MerchInfo.m_StrMerchCnName);
	}
	else
	{
		uParser.SetQueryValue(L"market",L"1");
		uParser.SetQueryValue(L"stockCode",L"600000");
		uParser.SetQueryValue(L"stockName",L"浦发银行");
	}

	return uParser.GetUrl();
}


void CIoViewNews::RefreshWeb( CString strUrl )
{
    m_wndCef.SetNeedReloadURL(true);
    m_wndCef.OpenUrl(strUrl);
}



void CIoViewNews::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	//
// 	lpMMI->ptMaxTrackSize.x = 140;
// 	lpMMI->ptMaxTrackSize.y = 391;
	CIoViewBase::OnGetMinMaxInfo(lpMMI);
}


CString CIoViewNews::GetLiveVideoUrl( CString StrOldUrl )
{
    CGGTongApp* pApp = (CGGTongApp*)AfxGetApp();
    if (NULL == pApp)
    {
        return StrOldUrl;
    }

    UrlParser uParser(StrOldUrl); 

    if (!pApp->m_bCustom)
    {
        CGGTongDoc *pDoc = pApp->m_pDocument;
        uParser.SetQueryValue(L"uname",pDoc->m_pAbsCenterManager->GetUserName());

        string strCode;
        CString tmpPwd = pDoc->m_pAbsCenterManager->GetUserPwd();
        UnicodeToUtf8(tmpPwd, strCode);

        unsigned char dst[1024];
        memset(dst, 0, sizeof(dst));
        unsigned char src[1024];
        memset(src, 0, sizeof(src));
        memcpy(src, strCode.c_str(), strCode.size());
        int32 dlen = sizeof(dst);
        int32 slen = strlen((const char *)src);
        CCodeFile::base64_encode(dst, &dlen, src, slen);

        USES_CONVERSION;
        uParser.SetQueryValue(L"pwd",_A2W((LPCSTR)dst));
    }
    return uParser.GetUrl();
}


void CIoViewNews::FrameExecuteJavaScript(char* pstrFun)
{
	m_wndCef.FrameExecuteJavaScript(pstrFun);
}

void CIoViewNews::JavaScriptResultValue(char* pstrFun)
{
	// 等有需要再处理这个返回值吧
}





void CIoViewNews::InitButtons()
{
	if (!m_StrShowName.IsEmpty())
	{
		m_iWebYPos = BTN_HEIGHT;

		CNCButton::T_NcFont font;
		font.m_StrName = gFontFactory.GetExistFontName(L"宋体");
		font.m_Size = 10.5;
		font.m_iStyle = FontStyleRegular;	

		CRect rtBtn(0, 0, 100, BTN_HEIGHT);
		m_BtnName.CreateButton(m_StrShowName, rtBtn, this, NULL, 0, BTN_NAME);
		m_BtnName.SetTextColor(m_clrNameText, m_clrNameText, m_clrNameText);
		m_BtnName.SetTextBkgColor(m_clrNameBk, m_clrNameBk, m_clrNameBk);
		m_BtnName.SetTextFrameColor(m_clrNameBk, m_clrNameBk, m_clrNameBk);
		m_BtnName.SetFont(font);
		m_BtnName.SetTextOffPos(CPoint(0,2));

		font.m_Size = 9;
		m_BtnMore.CreateButton(L"更多>>", rtBtn, this, NULL, 0, BTN_MORE);
		m_BtnMore.SetTextFrameColor(RGB(44,50,55), RGB(75,81,85), RGB(75,81,85));
		m_BtnMore.SetTextBkgColor(RGB(44,50,55), RGB(75,81,85), RGB(75,81,85));
		m_BtnMore.SetTextColor(RGB(201,208,214), RGB(255,255,255), RGB(255,255,255));
		m_BtnMore.SetFont(font);
	}
}

int32 CIoViewNews::TButtonHitTest(CPoint point)
{
	if (m_BtnMore.PtInButton(point) && m_BtnMore.GetCreate())
	{
		return m_BtnMore.GetControlId();
	}

	return INVALID_ID;
}

void CIoViewNews::ShowMoreInWsp()
{
	CMainFrame* pMain = DYNAMIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
	CIoViewNews *pIoViewNews = pMain->FindIoViewNews(true);
	CMPIChildFrame *pActiveFrame = DYNAMIC_DOWNCAST(CMPIChildFrame, pMain->MDIGetActive());
	if (NULL != pActiveFrame && pActiveFrame->GetIdString() != L"网页资讯" && pActiveFrame->IsLockedSplit())
	{
		// 当前页面无新闻页，且锁定，则调出默认报价表操作
		bool32 bLoad = pMain->m_pNewWndTB->DoOpenCfm(L"网页资讯");
		if ( bLoad )
		{
			pIoViewNews = pMain->FindIoViewNews(true);
		}
	}
	pIoViewNews->RefreshWeb(m_StrMoreUrl);
}

void CIoViewNews::ShowMoreInDlg()
{
	CGGTongApp* pApp = (CGGTongApp*)AfxGetApp();
	if (NULL == pApp)
	{
		return;
	}

	bool32 bShowIE = FALSE;
	int32 iSize = pApp->m_pConfigInfo->m_aWndSize.GetSize();
	int32 iWidth = 1024, iHeight = 700;
	CString StrName = m_StrShowName;
	for (int32 i=0; i<iSize; i++)
	{
		CWndSize wnd = pApp->m_pConfigInfo->m_aWndSize[i];
		if (m_StrShowName == wnd.m_strID)
		{
			StrName = wnd.m_strTitle;
			iWidth = wnd.m_iWidth;
			iHeight = wnd.m_iHeight;

			if (0 == iWidth || 0 == iHeight)
			{
				bShowIE = TRUE;
			}
			break;
		}
	}

	if (bShowIE)
	{
		ShellExecute(0, L"open", m_StrMoreUrl, NULL, NULL, SW_NORMAL);
	}
	else
	{
		CDlgTodayCommentIE::ShowDlgIEWithSize(StrName, m_StrMoreUrl, CRect(0, 0, iWidth, iHeight));
	}
}

void CIoViewNews::ShowMoreInExternal()
{
	ShellExecute(0, L"open", m_StrMoreUrl, NULL, NULL, SW_NORMAL);
}