#include "stdafx.h"
#include "memdc.h"
#include "IoViewSmartChooseStock.h"
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

// 智能选股界面上的三个标题啦高度
static int32 KCaptionHeight = 30;

/////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CIoViewSmartChooseStock, CIoViewBase)

///////////////////////////////////////////////////////////////////////////////
// message map
BEGIN_MESSAGE_MAP(CIoViewSmartChooseStock, CIoViewBase)
	//{{AFX_MSG_MAP(CIoViewQRCode)
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

///////////////////////////////////////////////////////////////////////////////
// 
CIoViewSmartChooseStock::CIoViewSmartChooseStock()
:CIoViewBase()
{
}

///////////////////////////////////////////////////////////////////////////////
// 
//lint --e{1540}
CIoViewSmartChooseStock::~CIoViewSmartChooseStock()
{
}

void CIoViewSmartChooseStock::OnPaint()
{
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

	//RectF fRect;
	//int32 iWidth = m_pImage->GetWidth();
	//int32 iHeight = m_pImage->GetHeight();
	//fRect.X = (REAL)(rcWindow.Width()-iWidth)/2;
	//fRect.Y = (REAL)(rcWindow.Height()-iHeight)/2;
	//fRect.Width = (REAL)iWidth;
	//fRect.Height = (REAL)iHeight;

	//graphics.DrawImage(m_pImage, fRect, 0, 0, (REAL)iWidth, (REAL)iHeight, UnitPixel);

	dc.BitBlt(0, 0, rcWindow.Width(), rcWindow.Height(), &memDC, 0, 0, SRCCOPY);
	dc.SelectClipRgn(NULL);
	memDC.DeleteDC();
	bmp.DeleteObject();

	m_ReportResult.RedrawWindow();
	m_ReportFlilterResult.RedrawWindow();
}

int CIoViewSmartChooseStock::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	CIoViewBase::OnCreate(lpCreateStruct);

	int iRet = CIoViewBase::OnCreate(lpCreateStruct);
	if ( -1 == iRet )
	{
		return -1;
	}

	//
	InitialIoViewFace(this);


	// 初始化各子窗口
	ASSERT( NULL != m_pAbsCenterManager );
	m_ReportResult.SetCenterManager(m_pAbsCenterManager);
	m_ReportFlilterResult.SetCenterManager(m_pAbsCenterManager);
	m_ReportResult.Create(WS_CHILD | WS_VISIBLE |SS_NOTIFY, this, CRect(0,0,0,0), 0x3010);
	m_ReportFlilterResult.Create(WS_CHILD| WS_VISIBLE |SS_NOTIFY, this, CRect(0,0,0,0), 0x3020);
	m_ReportResult.SetIsShowTab(FALSE);
	m_ReportFlilterResult.SetIsShowTab(FALSE);

	memcpyex(m_ReportResult.m_aIoViewColor,		m_aIoViewColor, sizeof(m_aIoViewColor));
	memcpyex(m_ReportFlilterResult.m_aIoViewColor, m_aIoViewColor, sizeof(m_aIoViewColor));
	memcpyex(m_ReportResult.m_aIoViewFont, m_aIoViewFont, sizeof(m_aIoViewFont));
	memcpyex(m_ReportFlilterResult.m_aIoViewFont, m_aIoViewFont, sizeof(m_aIoViewFont));

	//
	return iRet;
}

void CIoViewSmartChooseStock::OnSize(UINT nType, int cx, int cy) 
{
	CIoViewBase::OnSize(nType, cx, cy);
	RecalcLayout();
}

bool32	CIoViewSmartChooseStock::FromXml(TiXmlElement * pElement)
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

	m_ReportResult.SetReportType(CIoViewReportSelect::ERST_COMPLETE_RESULT);
	m_ReportFlilterResult.SetReportType(CIoViewReportSelect::ERST_FILTER_RESULT);
	
	return true;
}

CString	CIoViewSmartChooseStock::ToXml()
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

CString	CIoViewSmartChooseStock::GetDefaultXML()
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

void CIoViewSmartChooseStock::SetChildFrameTitle()
{
	CString StrTitle;
	StrTitle =  CIoViewManager::FindIoViewObjectByIoViewPtr(this)->m_StrLongName;
	
	CMPIChildFrame * pParentFrame = DYNAMIC_DOWNCAST(CMPIChildFrame, GetParentFrame());
	if ( NULL != pParentFrame)
	{
		pParentFrame->SetChildFrameTitle(StrTitle);
	}
}

void CIoViewSmartChooseStock::OnIoViewActive()
{
	CIoViewBase::OnIoViewActive();
	SetFocus();
	SetChildFrameTitle();
}

void CIoViewSmartChooseStock::OnIoViewDeactive()
{
	CIoViewBase::OnIoViewDeactive();
}

// 重新安排控件和视图的位置
void CIoViewSmartChooseStock::RecalcLayout()
{
	CRect rect;
	GetClientRect(&rect);
    if(rect.Height() <= 0)
	{
		return;
	}

	CRect rtReportResult, rtReportFlilterResult;
    rtReportResult = rect;
	rtReportResult.bottom = rect.top + rect.Height() / 2;
    rtReportFlilterResult = rect;
	rtReportFlilterResult.top = rtReportResult.bottom + 1;
	rtReportFlilterResult.bottom = rect.bottom;

	m_ReportResult.MoveWindow(&rtReportResult);
	m_ReportFlilterResult.MoveWindow(&rtReportFlilterResult);
	m_ReportResult.ShowWindow(SW_SHOW);
	m_ReportFlilterResult.ShowWindow(SW_SHOW);
}
