// Trade.cpp : implementation file
//

#include "stdafx.h"

#include "DlgCloseTradeTip.h"
#include "coding.h"
#include "FontFactory.h"

#define BK_COLOR		RGB(255, 255, 255)

#define		Title_Height	35
#define		COLOR_BUTTONT_BK			RGB(243, 243, 243)	// 关闭按钮的背景色

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

CDlgCloseTradeTip::CDlgCloseTradeTip(CWnd* pParent): CDialog(CDlgCloseTradeTip::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTrade)
	m_eTipType = ETT_NONE;
	m_strTipMsg = _T("");
	m_pCenterWnd = NULL;
	//}}AFX_DATA_INIT
}

void CDlgCloseTradeTip::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTrade)
	DDX_Control(pDX, IDC_STATIC_PIC, m_CtrlPic);
	DDX_Control(pDX, IDC_STATIC_TIP, m_staticTip);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDlgCloseTradeTip, CDialog)
	//{{AFX_MSG_MAP(CTrade)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTrade message handlers
CDlgCloseTradeTip::~CDlgCloseTradeTip()
{
}

BOOL CDlgCloseTradeTip::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_staticTip.SetBKColor(BK_COLOR);
	m_staticTip.SetFontSize(14);
	m_staticTip.SetTextColor(RGB(0x4D, 0x4D, 0x4D));

	m_buttonOk.CreateButton(L"重新登录", CRect(0, 0, 0, 0), this,NULL,3, IDC_BUTTON_OK);
	m_buttonOk.SetTextBkgColor(Color(0xED, 0x7C, 0x22),Color(200,124,50),Color(0xED, 0x7C, 0x22));
	m_buttonOk.SetTextFrameColor(Color(0xED, 0x7C, 0x22),Color(200,124,50),Color(0xED, 0x7C, 0x22));

	m_buttonCancel.CreateButton(L"注销", CRect(0, 0, 0, 0), this,NULL,3, IDC_BUTTON_CANCEL);
	m_buttonCancel.SetTextBkgColor(Color(216,215,215),Color(216,215,215),Color(216,215,215));
	m_buttonCancel.SetTextFrameColor(Color(216,215,215),Color(216,215,215),Color(216,215,215));

	// 关闭窗口 
	m_pBtnCloseWnd = new CButtonSTMain();
	m_pBtnCloseWnd->Create(L"",WS_CHILD | WS_VISIBLE,CRect(0,0,1,1),this, IDC_BUTTON_CLOSE);
	m_pBtnCloseWnd->SetIcon(IDI_ICON_QUIT_COLOR, IDI_ICON_QUIT_BLACK);
	m_pBtnCloseWnd->ShowWindow(SW_SHOW);
	m_pBtnCloseWnd->SetColor(CButtonSTMain::BTNST_COLOR_BK_IN, RGB(0xE2, 0xEA, 0xF4));
	m_pBtnCloseWnd->SetColor(CButtonSTMain::BTNST_COLOR_BK_OUT, RGB(0xE2, 0xEA, 0xF4));
	m_pBtnCloseWnd->SetColor(CButtonSTMain::BTNST_COLOR_BK_FOCUS, RGB(0xE2, 0xEA, 0xF4));
	m_pBtnCloseWnd->DrawBorder(FALSE);
	
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	m_staticTip.SetWindowText(m_strTipMsg);

	AdjustWindSize();

	if(m_pCenterWnd != NULL)	// 居中显示到传入窗口的中心处
	{
		CRect rc(0, 0, 0, 0), rect(0, 0, 0, 0), rcTemp(0, 0, 0, 0);
		m_pCenterWnd->GetWindowRect(rc);
		GetWindowRect(rect);
		rcTemp.left = rc.left + (rc.Width() - rect.Width()) / 2;
		rcTemp.top = rc.top + (rc.Height() - rect.Height()) / 2;
		rcTemp.right = rcTemp.left + rect.Width();
		rcTemp.bottom = rcTemp.top + rect.Height();
		
		MoveWindow(rcTemp);
	}
	else
	{
		CenterWindow();
	}

	return TRUE;
}

void CDlgCloseTradeTip::AdjustWindSize()
{
	int nWidth = 350, nHeight = 140, nSpace = 50 /*两个按钮之间的间距*/;
	::SetWindowPos(this->GetSafeHwnd(), NULL, 0, 0, nWidth, nHeight, SWP_NOZORDER | SWP_NOMOVE);
	CRect rcWindow, rcStatic, rcOk, rcCancel, rcClose;
	GetClientRect(rcWindow);
	// 调整静态文本框 
	m_staticTip.AdjustStaticHeight();
	m_staticTip.GetWindowRect(rcStatic);
	ScreenToClient(rcStatic);
	nWidth -= 40;	// 计算静态文本框的宽、高
	nHeight = rcStatic.Height();
	rcStatic.top = 40;
	rcStatic.bottom = rcStatic.top + nHeight;
	rcStatic.left = (rcWindow.Width() - nWidth) / 2;
	rcStatic.right = rcStatic.left + nWidth;
	m_staticTip.MoveWindow(rcStatic);
	// 确认按钮
	nWidth = 80;
	nHeight = 30;
	rcOk.top = rcStatic.bottom + 18;
	rcOk.bottom = rcOk.top + nHeight;
	rcOk.left = (rcWindow.Width() - nWidth * 2 - nSpace) / 2;
	rcOk.right = rcOk.left + nWidth;
	m_buttonOk.SetRect(rcOk);
	// 取消按钮
	rcCancel = rcOk;
	rcCancel.left = rcOk.right + nSpace;
	rcCancel.right = rcCancel.left + nWidth;
	m_buttonCancel.SetRect(rcCancel);
	// 关闭按钮
	nWidth = nHeight = 16;
	rcClose.right = rcWindow.right - 10;
	rcClose.left = rcClose.right - nWidth;
	rcClose.top = (Title_Height - nHeight) / 2;
	rcClose.bottom = rcClose.top + nHeight;
	m_pBtnCloseWnd->MoveWindow(rcClose);
	
	rcWindow.bottom = rcCancel.bottom + 10;	
	MoveWindow(rcWindow);
}

void CDlgCloseTradeTip::OnSysCommand(UINT nID, LPARAM lParam)
{
	CDialog::OnSysCommand(nID, lParam);
}

void CDlgCloseTradeTip::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting
		
		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);
		
		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;
		
		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CPaintDC dc(this);
		CRect rect;
		GetClientRect(rect);
		dc.FillSolidRect(rect, BK_COLOR);
		
		Graphics graphics(m_hWnd);
		// 绘制标题
		SolidBrush solidBrush(Color(0xFF, 0xE2, 0xEA, 0xF4));
		graphics.FillRectangle(&solidBrush, 0, 0, rect.Width(), Title_Height);
		SolidBrush solidText(Color(0xFF, 0x4B, 0x59, 0x6B));
		CFontFactory fontFactory;
		FontFamily fontFamily(fontFactory.GetExistFontName(L"微软雅黑"));//...
		Gdiplus::Font font(&fontFamily, 15, FontStyleRegular, UnitPixel);	//显示Name的字体
		StringFormat stringFormat;
		stringFormat.SetLineAlignment(StringAlignmentCenter);	//垂直方向居中显示
		stringFormat.SetTrimming(StringTrimmingNone);
		
		RectF rectF(float(rect.left + 20), float(rect.top), float(rect.Width()), Title_Height);
		graphics.DrawString(L"提示", -1, &font, rectF, &stringFormat, &solidText);
		
		CRect rcOk;
		m_buttonOk.GetRect(rcOk); 
		int nBottomHeight = 20 + rcOk.Height();
		RectF rectBottom(rect.left, rect.bottom - nBottomHeight, rect.Width(), nBottomHeight);
		graphics.FillRectangle(&solidBrush, rectBottom);
		// 绘制图标
		//Image* pImage = Image::FromFile(CPathFactory::GetImageMainIcon32Path());
		//graphics.DrawImage(pImage, 5, 10, 16, 16);
		m_buttonOk.DrawButton(&graphics);
		m_buttonCancel.DrawButton(&graphics);
		// 绘制边框
		Pen pen(Color(255, 0xD9, 0xD3, 0xC2));
		graphics.DrawRectangle(&pen, 0, 0, rect.Width() - 1, rect.Height() - 1);
		
		m_pBtnCloseWnd->RedrawWindow();

		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CDlgCloseTradeTip::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CDlgCloseTradeTip::OnSize( UINT nType, int cx, int cy )
{
	CDialog::OnSize(nType, cx, cy);
	
}

void CDlgCloseTradeTip::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (m_buttonOk.PtInButton(point))
	{
		m_buttonOk.LButtonUp();
	}
	else if (m_buttonCancel.PtInButton(point))
	{
		m_buttonCancel.LButtonUp();
	}
}

void CDlgCloseTradeTip::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (m_buttonOk.PtInButton(point))
	{
		m_buttonOk.LButtonDown();
	}
	else if (m_buttonCancel.PtInButton(point))
	{
		m_buttonCancel.LButtonDown();
	}
	
	CRect rect;
	GetClientRect(rect);
	rect.bottom = Title_Height;
	if(rect.PtInRect(point))
	{
		PostMessage(WM_NCLBUTTONDOWN, HTCAPTION, MAKELPARAM(point.x, point.y)); 
	}
}

void CDlgCloseTradeTip::OnMouseMove(UINT nFlags, CPoint point)
{
	if (m_buttonOk.PtInButton(point))
	{
		m_buttonOk.MouseHover();
	}
	else
	{
		m_buttonOk.MouseLeave();
	}
	
	if (m_buttonCancel.PtInButton(point))
	{
		m_buttonCancel.MouseHover();
	}
	else
	{
		m_buttonCancel.MouseLeave();
	}
}

BOOL CDlgCloseTradeTip::OnCommand(WPARAM wParam, LPARAM lParam)
{
	int32 iID = (int32)wParam;
	
	if (iID == IDC_BUTTON_OK)
	{
		OnOK();
		return true;
	}
	
	if(iID == IDC_BUTTON_CANCEL || iID == IDC_BUTTON_CLOSE)
	{
		OnCancel();
		return true;
	}
	
	return CDialog::OnCommand(wParam, lParam);
}