// DlgNews.cpp : implementation file
//
#include "stdafx.h"
#include "DlgDefaultUserRegister.h"
#include "DlgTodayComment.h"

#define INVALID_ID -1
#define ID_USER_BTN_CLOSE    12000

#ifdef _DEBUG
#define new DEBUG_NEW 
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
/////////////////////////////////////////////////////////////////////////////
CDlgDefaultUserRegister::CDlgDefaultUserRegister(CWnd* pParent /*=NULL*/)
: CDialog(CDlgDefaultUserRegister::IDD, pParent)
{
    m_pImgBk  = NULL;
	m_pImgBtn = NULL;
    m_iXButtonHovering = INVALID_ID;
}

CDlgDefaultUserRegister::~CDlgDefaultUserRegister()
{
	DEL(m_pImgBk);
	DEL(m_pImgBtn);
}

BEGIN_MESSAGE_MAP(CDlgDefaultUserRegister, CDialog)
    ON_WM_PAINT()
    ON_WM_ACTIVATE()
    ON_WM_MOUSEMOVE()
    ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave) 
    ON_WM_LBUTTONDOWN()
    ON_WM_LBUTTONUP()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

void CDlgDefaultUserRegister::InitButtons()
{
    m_pImgBtn = Image::FromFile(L"image//guanbi.png");
    CString strVal = "";

    int iWidth = m_pImgBtn->GetWidth();
    int iHeight= m_pImgBtn->GetHeight();
    int iLeftPos = 200;
    int iTopPos = 5;

    CRect rcBtn(iLeftPos, iTopPos, iLeftPos+iWidth, iTopPos+iHeight);

    CNCButton btnControl;
    btnControl.CreateButton(strVal, rcBtn, this, m_pImgBtn, 1, ID_USER_BTN_CLOSE);

    m_mapButton[ID_USER_BTN_CLOSE] = btnControl;
}

void CDlgDefaultUserRegister::OnPaint()
{
    CPaintDC dc(this);

    m_iXButtonHovering = INVALID_ID;
    Draw(dc);
}

void CDlgDefaultUserRegister::Draw( CDC& dc )
{
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

    if(m_pImgBk)
    {
        RectF destRect;
        destRect.X = 0;
        destRect.Y = 0;
        destRect.Width  = rcWindow.Width();
        destRect.Height = rcWindow.Height();
        graphics.DrawImage(m_pImgBk, destRect, 0, 0, m_pImgBk->GetWidth()-1, m_pImgBk->GetHeight(), UnitPixel);
    }

    CRect rcPaint;
    dc.GetClipBox(&rcPaint);
    map<int, CNCButton>::iterator iter;
    CRect rcControl;

    // 遍历首页工具栏上所有按钮
    for (iter=m_mapButton.begin(); iter!=m_mapButton.end(); ++iter)
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

    dc.BitBlt(0, 0, rcWindow.Width(), rcWindow.Height(), &memDC, 0, 0, SRCCOPY);
    dc.SelectClipRgn(NULL);
    memDC.DeleteDC();
    bmp.DeleteObject();
}

void CDlgDefaultUserRegister::DoDataExchange( CDataExchange* pDX )
{
    CDialog::DoDataExchange(pDX);
}

BOOL CDlgDefaultUserRegister::OnInitDialog()
{
    InitButtons();
    UpdateData(FALSE);
    m_pImgBk = Image::FromFile(L"image//registerAv.png");

    return TRUE;
}

void CDlgDefaultUserRegister::OnMouseMove( UINT nFlags, CPoint point )
{
    int iButton = TButtonHitTest(point);

    if (iButton != m_iXButtonHovering)
    {
        if (INVALID_ID != m_iXButtonHovering)
        {
            m_mapButton[m_iXButtonHovering].MouseLeave();
            m_iXButtonHovering = INVALID_ID;
        }

        if (INVALID_ID != iButton)
        {	
            m_iXButtonHovering = iButton;
            //m_mapButton[m_iXButtonHovering].MouseHover();
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

LRESULT CDlgDefaultUserRegister::OnMouseLeave( WPARAM, LPARAM )
{
    if (INVALID_ID != m_iXButtonHovering)
    {
        m_mapButton[m_iXButtonHovering].MouseLeave();
        m_iXButtonHovering = INVALID_ID;
    }

    return 0;    
}

void CDlgDefaultUserRegister::OnLButtonDown( UINT nFlags, CPoint point )
{
    int iButton = TButtonHitTest(point);

    if(INVALID_ID == iButton)
    {
        ShowNewsDlg();
    }
    PostMessage(WM_CLOSE, NULL, NULL);  

    CDialog::OnLButtonDown(nFlags, point);
}

void CDlgDefaultUserRegister::OnLButtonUp( UINT nFlags, CPoint point )
{
    int iButton = TButtonHitTest(point);
    if (INVALID_ID != iButton)
    {
        m_mapButton[iButton].LButtonUp();
    }

    CDialog::OnLButtonUp(nFlags, point);
}

int CDlgDefaultUserRegister::TButtonHitTest( CPoint point )
{
    map<int, CNCButton>::iterator iter;

    // 遍历标题栏上所有按钮
    for (iter = m_mapButton.begin(); iter!=m_mapButton.end(); ++iter)
    {
        CNCButton &btnControl = iter->second;

        // 点point是否在已绘制的按钮区域内
        if (btnControl.PtInButton(point)&& btnControl.GetCreate())
        {
            return btnControl.GetControlId();
        }
    }

    return INVALID_ID;
}

void CDlgDefaultUserRegister::ShowNewsDlg()
{
    CGGTongApp* pApp = (CGGTongApp*)AfxGetApp();
    if (NULL == pApp)
    {
        return ;
    }

	CString StrUrl = pApp->m_pConfigInfo->m_StrRegUrl;
	CString StrName = pApp->m_pConfigInfo->m_StrBtName;
	if (0 >= pApp->m_pConfigInfo->m_StrBtName.GetLength())
	{
		StrName = L"用户注册";
	}

	bool32 bShowIE = FALSE;
	int32 iSize = pApp->m_pConfigInfo->m_aWndSize.GetSize();
	int32 iWidth = 1024, iHeight = 700;
	for (int32 i=0; i<iSize; i++)
	{
		CWndSize wnd = pApp->m_pConfigInfo->m_aWndSize[i];
		if (L"注册" == wnd.m_strID)
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
		ShellExecute(0, L"open", StrUrl, NULL, NULL, SW_NORMAL);
	}
	else
	{
		CDlgTodayCommentIE::ShowDlgIEWithSize(StrName, StrUrl, CRect(0, 0, iWidth, iHeight));
	}
}

void CDlgDefaultUserRegister::OnActivate( UINT nState, CWnd* pWndOther, BOOL bMinimized )
{
    CDialog::OnActivate(nState, pWndOther, bMinimized);  
    if (WA_INACTIVE == nState)  
    {  
        //PostMessage(WM_CLOSE, NULL, NULL);  
    }  
}
