// Trade.cpp : implementation file
//
#include "stdafx.h"
#include "DlgTip.h"
#include "coding.h"
#include "PathFactory.h"
#include "FontFactory.h"

#define		COLOR_BUTTONT_BK			RGB(243, 243, 243)	// 关闭按钮的背景色

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define BK_COLOR		RGB(255, 255, 255)

#define Title_Height	35
/////////////////////////////////////////////////////////////////////////////

CDlgTip::CDlgTip(CWnd* pParent /*=NULL*/) : CDialog(CDlgTip::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTrade)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_eTipType = ETT_NONE;
	m_strTipMsg = _T("");
	m_pCenterWnd = NULL;
	m_pos.x = m_pos.y = -1;
	m_bCenterWnd = TRUE;
}

void CDlgTip::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);	
	//{{AFX_DATA_MAP(CTrade)
	DDX_Control(pDX, IDC_STATIC_TIPMSG, m_staticTip);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDlgTip, CDialog)
	//{{AFX_MSG_MAP(CTrade)
		// NOTE: the ClassWizard will add message map macros here
		//}}AFX_MSG_MAP
		ON_WM_SYSCOMMAND()
		ON_WM_PAINT()
		ON_WM_LBUTTONDOWN()
		ON_WM_LBUTTONUP()
		ON_WM_MOUSEMOVE()
		ON_WM_QUERYDRAGICON()
	ON_WM_SIZE()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTrade message handlers
CDlgTip::~CDlgTip()
{
}

BOOL CDlgTip::OnInitDialog()
{
	CDialog::OnInitDialog();

	InitControl();

	AdjustControl();
 
	if (m_pos.x != -1 && m_pos.y != -1)
	{		
		::SetWindowPos(GetSafeHwnd(), NULL, m_pos.x, m_pos.y, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
	}
	else
	{
		// 使当前窗口的位置处于m_pCenterWnd窗口的中心
		SetWindowToCenter();
	}

	return TRUE;
}

void CDlgTip::AdjustControl()
{
	int nWidth = 220, nHeight = 180;
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
	nWidth = int(rcWindow.Width() * 0.5f);
	nHeight = 30;
	rcOk.top = rcStatic.bottom + 18;
	rcOk.bottom = rcOk.top + nHeight;
	rcOk.left = rcStatic.left;
	rcOk.right = rcOk.left + nWidth;
	m_buttonOk.SetRect(rcOk);
	// 取消按钮
	rcCancel.top = rcOk.top;
	rcCancel.bottom = rcOk.bottom;
	rcCancel.right = rcStatic.right;
	rcCancel.left = rcCancel.right - nWidth;
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

	m_buttonCancel.SetVisiable(false);
	// 如果取消按钮是隐藏的，则确定按钮居中显示
	if(!m_buttonCancel.IsVisiable())
	{
		nWidth = rcOk.Width();
		rcOk.left = rcWindow.left + (rcWindow.Width() - nWidth) / 2;
		rcOk.right = rcOk.left + nWidth;
		m_buttonOk.SetRect(rcOk);
	}
}

void CDlgTip::OnSysCommand(UINT nID, LPARAM lParam)
{
	CDialog::OnSysCommand(nID, lParam);
}

void CDlgTip::OnPaint() 
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
		CString strText;
		GetWindowText(strText);
		graphics.DrawString(strText, -1, &font, rectF, &stringFormat, &solidText);

		CRect rcOk;
		m_buttonOk.GetRect(rcOk); 
		int nBottomHeight = 20 + rcOk.Height();
		RectF rectBottom(rect.left, rect.bottom - nBottomHeight, rect.Width(), nBottomHeight);
		graphics.FillRectangle(&solidBrush, rectBottom);
		// 绘制图标
// 		Image* pImage = Image::FromFile(CPathFactory::GetImageMainIcon32Path());
// 		graphics.DrawImage(pImage, 5, 10, 16, 16);
 		m_buttonOk.DrawButton(&graphics);
// 		m_buttonCancel.DrawButton(&graphics);
		// 绘制边框
		Pen pen(Color(255, 0xD9, 0xD3, 0xC2));
		graphics.DrawRectangle(&pen, 0, 0, rect.Width() - 1, rect.Height() - 1);

		m_pBtnCloseWnd->RedrawWindow();

		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CDlgTip::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CDlgTip::OnSize( UINT nType, int cx, int cy )
{
	CDialog::OnSize(nType, cx, cy);	
}

void CDlgTip::OnLButtonUp(UINT nFlags, CPoint point)
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

void CDlgTip::OnLButtonDown(UINT nFlags, CPoint point)
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

void CDlgTip::OnMouseMove(UINT nFlags, CPoint point)
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

BOOL CDlgTip::OnCommand(WPARAM wParam, LPARAM lParam)
{
	int32 iID = (int32)wParam;

	if (iID == IDC_BUTTON_OK)
	{
		OnOK();
		return true;
	}

	if(iID == IDC_BUTTON_CANCEL || iID == IDCANCEL)
	{
		if (m_pCenterWnd && !m_pCenterWnd->IsWindowVisible())
		{
			m_pCenterWnd->ShowWindow(SW_SHOW);
		}
		OnCancel();
		return true;
	}
	
	return CDialog::OnCommand(wParam, lParam);
}

void CDlgTip::AdjustWindowPos(int x, int y)
{
	m_pos.x = x;
	m_pos.y = y;
}

void CDlgTip::SetWindowToCenter()
{
	if(m_pCenterWnd != NULL)	// 居中显示到传入窗口的中心处
	{
		if (!m_pCenterWnd->IsWindowVisible())
		{
			CenterWindow();
			return;
		}

		CRect rc, rect, rcTemp;
		m_pCenterWnd->GetWindowRect(rc);
		GetWindowRect(rect);
		rcTemp.left = rc.left + (rc.Width() - rect.Width()) / 2;
		rcTemp.top = rc.top + (rc.Height() - rect.Height()) / 2;
		rcTemp.right = rcTemp.left + rect.Width();
		rcTemp.bottom = rcTemp.top + rect.Height();
		// 判断一下弹窗的位置是否在屏幕可视位置
		const INT nWidth = GetSystemMetrics(SM_CXSCREEN);
		const INT nHeight = GetSystemMetrics(SM_CYSCREEN);
		//if(rcTemp.left > nWidth || rcTemp.top > nHeight || rcTemp.right < 0 || rcTemp.bottom < 0)
		if(rcTemp.left < 0 || rcTemp.top < 0 || rcTemp.right > nWidth || rcTemp.bottom > nHeight)
		{
			CenterWindow();
		}
		else
		{
			MoveWindow(rcTemp);
		}
	}
}

void CDlgTip::InitControl()
{
	m_staticTip.SetBKColor(BK_COLOR);
	m_staticTip.SetFontSize(14);
	m_staticTip.SetTextColor(RGB(0x4D, 0x4D, 0x4D));

	m_buttonOk.CreateButton(L"确认", CRect(0, 0, 0, 0), this,NULL,3, IDOK);
	m_buttonOk.SetTextBkgColor(Color(0xED, 0x7C, 0x22),Color(200,124,50),Color(0xED, 0x7C, 0x22));
	m_buttonOk.SetTextFrameColor(Color(0xED, 0x7C, 0x22),Color(200,124,50),Color(0xED, 0x7C, 0x22));

	m_buttonCancel.CreateButton(L"取消", CRect(0, 0, 0, 0), this,NULL,3, IDC_BUTTON_CANCEL);
	m_buttonCancel.SetTextBkgColor(Color(0xED, 0x7C, 0x22),Color(216,215,215),Color(0xED, 0x7C, 0x22));
	m_buttonCancel.SetTextFrameColor(Color(0xED, 0x7C, 0x22),Color(216,215,215),Color(0xED, 0x7C, 0x22));

	// 关闭窗口 
	m_pBtnCloseWnd = new CButtonSTMain();
	m_pBtnCloseWnd->Create(L"",WS_CHILD | WS_VISIBLE,CRect(0,0,1,1),this, IDCANCEL);
	m_pBtnCloseWnd->SetIcon(IDI_ICON_QUIT_COLOR, IDI_ICON_QUIT_BLACK);
	m_pBtnCloseWnd->ShowWindow(SW_SHOW);
	m_pBtnCloseWnd->SetColor(CButtonSTMain::BTNST_COLOR_BK_IN, RGB(0xE2, 0xEA, 0xF4));
	m_pBtnCloseWnd->SetColor(CButtonSTMain::BTNST_COLOR_BK_OUT, RGB(0xE2, 0xEA, 0xF4));
	m_pBtnCloseWnd->SetColor(CButtonSTMain::BTNST_COLOR_BK_FOCUS, RGB(0xE2, 0xEA, 0xF4));
	m_pBtnCloseWnd->DrawBorder(FALSE);

	m_buttonCancel.SetVisiable(FALSE);

	if (m_eTipType == ETT_TIP)
	{
		m_buttonCancel.SetVisiable(TRUE);
	}

	CString strFiled = _T("");

	//图标 2014-02-21 by cym
	CString StrIconPath = CPathFactory::GetImageMainIcon32Path();

	m_hIcon = (HICON)LoadImage(AfxGetInstanceHandle(), StrIconPath, IMAGE_ICON, 0, 0, LR_DEFAULTSIZE|LR_LOADFROMFILE);

	if ( NULL == m_hIcon )
	{
		m_hIcon = LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME));
	}

	if (m_eTipType==ETT_SUC)//成功
	{
		this->SetWindowText(L"提示");
		m_hIconDlg = AfxGetApp()->LoadIcon(IDI_ICON_SUC);
	}
	else if (m_eTipType==ETT_ERR)//失败
	{
		this->SetWindowText(L"错误");
		m_hIconDlg = AfxGetApp()->LoadIcon(IDI_ICON_ERR);	
	}
	else if (m_eTipType==ETT_WAR)//警告
	{
		this->SetWindowText(L"提示");
		m_hIconDlg = AfxGetApp()->LoadIcon(IDI_ICON_WAR);
	}
	else if (m_eTipType==ETT_TIP)//提示
	{
		if(m_strTitle.IsEmpty())
		{
			this->SetWindowText(L"提示");
		}
		else
		{
			this->SetWindowText(m_strTitle);
		}
		m_hIconDlg = AfxGetApp()->LoadIcon(IDI_ICON_TIP);
	}
	else
	{
		return;
	}

	GetDlgItem(IDC_STATIC_TIPMSG)->SetWindowText(m_strTipMsg);
}

void CDlgTip::ShowCenterWnd( BOOL bShow )
{
	m_bCenterWnd =bShow;
}
