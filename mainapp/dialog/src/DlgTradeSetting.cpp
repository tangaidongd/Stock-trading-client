// DlgTradeSetting.cpp : implementation file
//
#include "stdafx.h"

#include "TraceLog.h"
#include "DlgTradeSetting.h"
#include "GGTong.h"
#include "pathfactory.h"
#include "tinyxml.h"
#include "FontFactory.h"
#include "ConfigInfo.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define INVALID_ID			-1
#define ID_TRADESETTING_CLOSE		10001 

/////////////////////////////////////////////////////////////////////////////
// CDlgTradeSetting dialog

CDlgTradeSetting::CDlgTradeSetting( CWnd* pParent /*=NULL*/)
	: CDialog(CDlgTradeSetting::IDD, pParent)
{
	m_iNumOfPage        = 0;
	m_pDlgTradeSetup    = new CDlgTradeSetup(this);//this即empty对话框的窗口指针，就是说新建一个子对象指针
	m_pDlgTradeDownload = new CDlgTradeDownload(this);
	m_iXButtonHovering = INVALID_ID;
}

CDlgTradeSetting::~CDlgTradeSetting()
{
	DEL(m_pImgCaption);
	DEL(m_pImgClose);

	DEL(m_pDlgTradeSetup);
	DEL(m_pDlgTradeDownload);
	
	m_fontStaticText.DeleteObject();
	m_fontCheckText.DeleteObject();
}

void CDlgTradeSetting::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgTradeSetting)

	//}}AFX_DATA_MAP

}

BEGIN_MESSAGE_MAP(CDlgTradeSetting, CDialog)
	//{{AFX_MSG_MAP(CDlgTradeSetting)
	//
	ON_WM_TIMER()
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_MOUSEMOVE()
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave) 
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_NCHITTEST()
	ON_WM_CTLCOLOR()
	ON_WM_LBUTTONDBLCLK()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgTradeSetting message handlers

BOOL CDlgTradeSetting::OnInitDialog() 
{
	int32 iTitleHeight = 60;
	int32 iWndWidth = 570;
	int32 iWndHeight = 400;
	int32 iTopHeight = 209;
	CRect rcWnd(0, 0, iWndWidth, iWndHeight);

	m_rcCaption = rcWnd;
	m_rcCaption.bottom = iTitleHeight;

	MoveWindow(rcWnd);

	m_pImgClose = Image::FromFile(CPathFactory::GetImageLoginClosePath());
	int iHeight = m_rcCaption.top + 6;
	CRect rcTmp(rcWnd.right-m_pImgClose->GetWidth()-5, iHeight, rcWnd.right-5, iHeight + m_pImgClose->GetHeight()/3);
	AddButton(rcTmp, m_pImgClose, 3, ID_TRADESETTING_CLOSE);	//关闭按钮

	m_pImgCaption  = Image::FromFile(L"image//CapitonTrade.png");

 	m_rcShowRect.SetRect(m_rcCaption.left, m_rcCaption.bottom , rcWnd.right, rcWnd.bottom);
	
 	this->AddPage(m_pDlgTradeSetup, IDD_DIALOG_TRADE_SETUP);
 	this->AddPage(m_pDlgTradeDownload, IDD_DIALOG_TRADE_DOWNLOAD);
 
	this->ShowPage(0);//把main对话框显示出来
 	
	CenterWindow();
	
	return TRUE; 
}



BOOL CDlgTradeSetting::AddPage(CDialog* pDialog, UINT ID)
{
	m_pPages[m_iNumOfPage] = pDialog;
	m_uIDD[m_iNumOfPage] = ID;
	m_pPages[m_iNumOfPage]->Create(ID, this);
	
	m_pPages[m_iNumOfPage]->MoveWindow(&m_rcShowRect);
	m_iNumOfPage++;
	
	return true;
}
void CDlgTradeSetting::ShowPage(int CurrentPage)
{
	for (int nCount = 0; nCount<m_iNumOfPage; nCount++)
	{
		if (nCount == CurrentPage)
		{
			m_pPages[nCount]->ShowWindow(SW_SHOW);
			
			if (0 == nCount)
			{
				m_pDlgTradeSetup->ShowChildWindow();
			}

			continue;
		}

		m_pPages[nCount]->ShowWindow(SW_HIDE);
	}
}

void CDlgTradeSetting::DrawTitleBar()
{
	CWindowDC dc(this);
	CDC memDC;
	memDC.CreateCompatibleDC(&dc);
	CBitmap bmp;

	bmp.CreateCompatibleBitmap(&dc, m_rcCaption.Width(), m_rcCaption.Height());
	memDC.SelectObject(&bmp);
	memDC.SetBkMode(TRANSPARENT);
	Gdiplus::Graphics graphics(memDC.GetSafeHdc());

	// caption
	RectF fRect;
	fRect.X = (REAL)m_rcCaption.left;
	fRect.Y = (REAL)m_rcCaption.top;
	fRect.Width = (REAL)m_rcCaption.Width();
	fRect.Height = (REAL)m_rcCaption.Height();
	if (NULL != m_pImgCaption)
	{
		graphics.DrawImage(m_pImgCaption, fRect, 0, 0, fRect.Width, fRect.Height, UnitPixel);
	}


	// 标题栏文本
	{
		RectF grect;
		grect.X = (REAL)25;
		grect.Y = (REAL)1;
		grect.Width = (REAL)80;
		grect.Height = (REAL)fRect.Height;

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
		T_NcFont m_Font;
		m_Font.m_StrName = gFontFactory.GetExistFontName(L"微软雅黑");	//...
		m_Font.m_Size	 = 20;
		m_Font.m_iStyle	 = FontStyleBold;	
		Gdiplus::FontFamily fontFamily(m_Font.m_StrName);
		Gdiplus::Font font(&fontFamily, m_Font.m_Size, m_Font.m_iStyle, UnitPoint);

		RectF rcBound;
		PointF point;
		CString StrCaption = L"交易设置";
		graphics.MeasureString(StrCaption, StrCaption.GetLength(), &font, point, &strFormat, &rcBound);
		grect.X = (m_rcCaption.Width() - rcBound.Width)/2;
		grect.Width = rcBound.Width;

		SolidBrush brush((ARGB)Color::White);
		brush.SetColor(Color::Color(255, 255, 255));
		graphics.DrawString(StrCaption, StrCaption.GetLength(), &font, grect, &strFormat, &brush);
	}

	CRect rcPaint;
	dc.GetClipBox(&rcPaint);
	map<int, CNCButton>::iterator iter;
	CRect rcControl;

	for (iter=m_mapBtn.begin(); iter!=m_mapBtn.end(); ++iter)
	{
		CNCButton &btnControl = iter->second;
		btnControl.GetRect(rcControl);

		UINT ID = btnControl.GetControlId();

		// 判断当前按钮是否需要重绘
		if (!rcPaint.IsRectEmpty() && !CRect().IntersectRect(&rcControl, rcPaint))
		{
			continue;
		}

		if (ID_TRADESETTING_CLOSE == ID )
		{
			btnControl.DrawButton(&graphics);
		}
	}

	dc.BitBlt(m_rcCaption.left, m_rcCaption.top, m_rcCaption.Width(), m_rcCaption.Height(), &memDC, 0, 0, SRCCOPY);
	dc.SelectClipRgn(NULL);
	memDC.DeleteDC();
	bmp.DeleteObject();
}

BOOL CDlgTradeSetting::OnEraseBkgnd(CDC* pDC)
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

	// caption
	RectF fRect;
	fRect.X = (REAL)m_rcCaption.left;
	fRect.Y = (REAL)m_rcCaption.top;
	fRect.Width = (REAL)m_rcCaption.Width();
	fRect.Height = (REAL)m_rcCaption.Height();
	if (NULL != m_pImgCaption)
	{
		graphics.DrawImage(m_pImgCaption, fRect, 0, 0, fRect.Width, fRect.Height, UnitPixel);
	}

	//	DrawLogo(graphics, fRect);

	// 标题栏文本
	{
		RectF grect;
		grect.X = (REAL)25;
		grect.Y = (REAL)1;
		grect.Width = (REAL)80;
		grect.Height = (REAL)fRect.Height;
		

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
		T_NcFont m_Font;
		m_Font.m_StrName = gFontFactory.GetExistFontName(L"微软雅黑");//...
		m_Font.m_Size	 = 20;
		m_Font.m_iStyle	 = FontStyleBold;	
		Gdiplus::FontFamily fontFamily(m_Font.m_StrName);
		Gdiplus::Font font(&fontFamily, m_Font.m_Size, m_Font.m_iStyle, UnitPoint);

		RectF rcBound;
		PointF point;
		CString StrCaption = L"交易设置";
		graphics.MeasureString(StrCaption, StrCaption.GetLength(), &font, point, &strFormat, &rcBound);
		grect.X = (m_rcCaption.Width() - rcBound.Width)/2;
		grect.Width = rcBound.Width;


		// 绘制标题
		SolidBrush brush((ARGB)Color::White);
		brush.SetColor(Color::Color(255, 255, 255));
		graphics.DrawString(StrCaption, StrCaption.GetLength(), &font, grect, &strFormat, &brush);
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

void CDlgTradeSetting::OnMouseMove(UINT nFlags, CPoint point) 
{
	int32 iButton = TButtonHitTest(point);

	if (iButton != m_iXButtonHovering)
	{
		if (INVALID_ID != m_iXButtonHovering)
		{
			if (ID_TRADESETTING_CLOSE==iButton )
			{
				m_mapBtn[m_iXButtonHovering].MouseLeave(FALSE);
				DrawTitleBar();
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
			if (ID_TRADESETTING_CLOSE==iButton )
			{
				m_mapBtn[m_iXButtonHovering].MouseHover(FALSE);
				DrawTitleBar();
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

LRESULT CDlgTradeSetting::OnMouseLeave(WPARAM wParam, LPARAM lParam)       
{     
	if (INVALID_ID != m_iXButtonHovering)
	{
		if (ID_TRADESETTING_CLOSE==m_iXButtonHovering )
		{
			m_mapBtn[m_iXButtonHovering].MouseLeave(FALSE);
			DrawTitleBar();
		}
		else
		{
			m_mapBtn[m_iXButtonHovering].MouseLeave();
		}
		m_iXButtonHovering = INVALID_ID;
	}

	return 0;       
} 

void CDlgTradeSetting::OnLButtonDown(UINT nFlags, CPoint point) 
{
	int32 iButton = TButtonHitTest(point);
	if (INVALID_ID != iButton)
	{
		if (ID_TRADESETTING_CLOSE==iButton )
		{
			m_mapBtn[iButton].LButtonDown(FALSE);
			DrawTitleBar();
		}
		else
		{
			m_mapBtn[iButton].LButtonDown();
		}
	}
}

void CDlgTradeSetting::OnLButtonUp(UINT nFlags, CPoint point) 
{
	int32 iButton = TButtonHitTest(point);
	if (INVALID_ID != iButton)
	{
		if (ID_TRADESETTING_CLOSE==iButton )
		{
			m_mapBtn[iButton].LButtonUp(FALSE);
			DrawTitleBar();
		}
		else
		{
			m_mapBtn[iButton].LButtonUp();
		}
	}
}

BOOL CDlgTradeSetting::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	int32 iID = (int32)wParam;
	std::map<int, CNCButton>::iterator it = m_mapBtn.find(iID);
	if (m_mapBtn.end() != it)
	{
		switch (iID)
		{
		case ID_TRADESETTING_CLOSE:
			{	
				// 关闭
				OnCancel();
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

void CDlgTradeSetting::OnPaint() 
{
	CPaintDC dc(this);
}

void CDlgTradeSetting::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	OnLButtonDown(nFlags, point);
	CDialog::OnLButtonDblClk(nFlags, point);
}

HBRUSH CDlgTradeSetting::OnCtlColor( CDC* pDC, CWnd* pWnd, UINT nCtlColor )
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	
	if (nCtlColor == CTLCOLOR_STATIC)
	{	
		CString s;
		pWnd->GetWindowText(s);
		
		pDC->SetBkMode(TRANSPARENT);
		pDC->SetTextColor(RGB(48,48,48));
		pDC->SelectObject(&m_fontStaticText);

		int32 iID = pWnd->GetDlgCtrlID();
		if (IDC_EDIT_USER!=iID && IDC_EDIT_PWD!=iID)
		{
			return (HBRUSH)GetStockObject(NULL_BRUSH);
		}
	}
	else if ( CTLCOLOR_BTN == nCtlColor )
	{
		if ( IDC_CHECK_SAVEPASSWORD == pWnd->GetDlgCtrlID() || IDC_CHECK_OPTIMIZE_SERVER == pWnd->GetDlgCtrlID() )
		{
			//return (HBRUSH)GetStockObject(NULL_BRUSH);
		}
	}

	return hbr;
}

void CDlgTradeSetting::AddButton(LPRECT lpRect, Image *pImage, UINT nCount, UINT nID, LPCTSTR lpszCaption)
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
int	 CDlgTradeSetting::TButtonHitTest(CPoint point)
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

LRESULT CDlgTradeSetting::OnNcHitTest(CPoint point) 
{	
	map<int, CNCButton>::iterator iter;
	ScreenToClient(&point);
	
	// 遍历所有按钮
	for (iter=m_mapBtn.begin(); iter!=m_mapBtn.end(); ++iter)
	{
		CNCButton &btnControl = iter->second;
		
		// 点point是否在已绘制的按钮区域内
		if (btnControl.PtInButton(point))
		{
			return HTCLIENT;
		}
	}

	return HTCAPTION;
}

BOOL CDlgTradeSetting::PreTranslateMessage(MSG* pMsg) 
{
	return BaseDialog::PreTranslateMessage(pMsg);
}
