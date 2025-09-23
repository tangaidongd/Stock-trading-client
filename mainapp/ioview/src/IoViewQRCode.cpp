#include "stdafx.h"
#include "memdc.h"
#include "IoViewQRCode.h"
#include "IoViewManager.h"
#include "PathFactory.h"
#include "WebClient.h"

//#pragma optimize("g", off)	// necessary in VC 6.0 to suppress (release mode)
							//    "fatal error C1001: INTERNAL COMPILER ERROR"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CIoViewQRCode, CIoViewBase)

///////////////////////////////////////////////////////////////////////////////
// message map
BEGIN_MESSAGE_MAP(CIoViewQRCode, CIoViewBase)
	//{{AFX_MSG_MAP(CIoViewQRCode)
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
	ON_WM_RBUTTONUP()
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

///////////////////////////////////////////////////////////////////////////////
// 
CIoViewQRCode::CIoViewQRCode()
:CIoViewBase()
{
	m_pImage     = NULL;
	m_StrAdvsUrl = L"";
	//--- wangyongxue 目前使用本地图片进行显示
	CGGTongApp* pApp = (CGGTongApp*)AfxGetApp();
	CString strAdvsCode = pApp->m_pConfigInfo->m_StrTopRightAdvsCode;
	CString StrAdvsUrl  = pApp->m_pConfigInfo->GetAdvsCenterUrlByCode(strAdvsCode,EAUTPit);
	if (!StrAdvsUrl.IsEmpty())
	{
		WebClient webClient;
		webClient.SetTimeouts(100, 200);
		webClient.DownloadFile(StrAdvsUrl,  L"image\\AdvsPic.png");
	}
}

///////////////////////////////////////////////////////////////////////////////
// 
//lint --e{1540}
CIoViewQRCode::~CIoViewQRCode()
{
	DEL(m_pImage);
}

///////////////////////////////////////////////////////////////////////////////
BOOL CIoViewQRCode::PreTranslateMessage(MSG* pMsg)
{
	return CIoViewBase::PreTranslateMessage(pMsg);
}

void CIoViewQRCode::OnPaint()
{
	if(NULL == m_pImage)
	{
		return;
	}

	CPaintDC dc(this);

	CDC memDC;
	memDC.CreateCompatibleDC(&dc);
	CBitmap bmp;
	CRect rcWindow;
	GetWindowRect(&rcWindow);
	rcWindow.OffsetRect(-rcWindow.left, -rcWindow.top);
	
	bmp.CreateCompatibleBitmap(&dc, rcWindow.Width(), rcWindow.Height());
	memDC.SelectObject(&bmp);
	memDC.SetBkMode(TRANSPARENT);
	Gdiplus::Graphics graphics(memDC.GetSafeHdc());

	RectF fRect;
	int32 iWidth = m_pImage->GetWidth();
	int32 iHeight = m_pImage->GetHeight();
	fRect.X = (REAL)(rcWindow.Width()-iWidth)/2;
	fRect.Y = (REAL)(rcWindow.Height()-iHeight)/2;
	fRect.Width = (REAL)iWidth;
	fRect.Height = (REAL)iHeight;

	graphics.DrawImage(m_pImage, fRect, 0, 0, (REAL)iWidth, (REAL)iHeight, UnitPixel);

	dc.BitBlt(0, 0, rcWindow.Width(), rcWindow.Height(), &memDC, 0, 0, SRCCOPY);
	dc.SelectClipRgn(NULL);
	memDC.DeleteDC();
	bmp.DeleteObject();
}

int CIoViewQRCode::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	CIoViewBase::OnCreate(lpCreateStruct);
	m_pImage = Image::FromFile(L".//image//AdvsPic.png");
	
	return 0;
}

void CIoViewQRCode::OnSize(UINT nType, int cx, int cy) 
{
	CIoViewBase::OnSize(nType, cx, cy);
}

bool32	CIoViewQRCode::FromXml(TiXmlElement * pElement)
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
	
	return true;
}

CString	CIoViewQRCode::ToXml()
{
	CString StrThis;
	
	// 
	StrThis.Format(L"<%s %s=\"%s\" %s=\"%s\" ",/*>\n*/ // %s=\"%s\" %s=\"%s\" 
		CString(GetXmlElementValue()).GetBuffer(), 
		CString(GetXmlElementAttrShowTabName()).GetBuffer(), 
		m_StrTabShowName.GetBuffer(),
		CString(GetXmlElementAttrIoViewType()).GetBuffer(), 
		CIoViewManager::GetIoViewString(this).GetBuffer()
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

CString	CIoViewQRCode::GetDefaultXML()
{
	CString StrThis;
	
	// 
	StrThis.Format(L"<%s %s=\"%s\"  >\n", //%s=\"%s\" %s=\"%s\"
		CString(GetXmlElementValue()).GetBuffer(), 
		CString(GetXmlElementAttrIoViewType()).GetBuffer(), 
		CIoViewManager::GetIoViewString(this).GetBuffer());
	//
	
	//
	StrThis += L"</";
	StrThis += GetXmlElementValue();
	StrThis += L">\n";
	
	return StrThis;
}

void CIoViewQRCode::SetChildFrameTitle()
{
	CString StrTitle;
	StrTitle =  CIoViewManager::FindIoViewObjectByIoViewPtr(this)->m_StrLongName;
	
	CMPIChildFrame * pParentFrame = DYNAMIC_DOWNCAST(CMPIChildFrame, GetParentFrame());
	if ( NULL != pParentFrame)
	{
		pParentFrame->SetChildFrameTitle(StrTitle);
	}
}

void CIoViewQRCode::OnIoViewActive()
{
	CIoViewBase::OnIoViewActive();
	SetFocus();
	SetChildFrameTitle();
}

void CIoViewQRCode::OnIoViewDeactive()
{
	CIoViewBase::OnIoViewDeactive();
}

// 通知视图改变关注的商品
void CIoViewQRCode::OnVDataMerchChanged(int32 iMarketId, const CString &StrMerchCode, IN CMerch *pMerch)
{
}

void CIoViewQRCode::OnRButtonUp(UINT nFlags, CPoint point)
{
	// 判断页面是否被锁定（锁定了就不弹出右键菜单）
	if (!IsLockedSplit())
	{
		CNewMenu Menu;
		Menu.LoadMenu(IDR_MENU_QRCODE);
		Menu.LoadToolBar(g_awToolBarIconIDs);// 为返回菜单加载图标

		CNewMenu* pPopMenu = DYNAMIC_DOWNCAST(CNewMenu, Menu.GetSubMenu(0));

		CMenu* pTempMenu = pPopMenu->GetSubMenu(L"插入内容");
		CNewMenu * pIoViewPopMenu = DYNAMIC_DOWNCAST(CNewMenu, pTempMenu );
		ASSERT(NULL != pIoViewPopMenu );
		CIoViewBase::AppendIoViewsMenu(pIoViewPopMenu, IsLockedSplit());

		CPoint pt;
		GetCursorPos(&pt);
		pPopMenu->TrackPopupMenu(TPM_LEFTALIGN|TPM_LEFTBUTTON|TPM_VERTICAL,pt.x, pt.y, AfxGetMainWnd());	
		pPopMenu->DestroyMenu();
	}

	CWnd::OnRButtonUp(nFlags, point);
}

void CIoViewQRCode::OnLButtonDown(UINT nFlags, CPoint point)
{
	CGGTongApp* pApp = (CGGTongApp*)AfxGetApp();
	CString StrAdvsCode = pApp->m_pConfigInfo->m_StrTopRightAdvsCode;
	m_StrAdvsUrl = pApp->m_pConfigInfo->GetAdvsCenterUrlByCode(StrAdvsCode,EAUTLink);

	if(!m_StrAdvsUrl.IsEmpty())
	{
		ShellExecute(0, L"open", m_StrAdvsUrl, NULL, NULL, SW_NORMAL);
	}

	CWnd::OnLButtonDown(nFlags, point);
}
