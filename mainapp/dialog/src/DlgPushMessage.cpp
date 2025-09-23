// DlgNews.cpp : implementation file
//
#include "stdafx.h"
#include "DlgPushMessage.h"
#include "DlgTodayComment.h"
#include "FontFactory.h"

#define INVALID_ID           -1
#define ID_MY_BTN_CLOSE    12000
#define ID_MY_BTN_ORDER    12001	// �����µ�

const int32 iTypeLeftSpace = 16;
const int32 iTypeTopSpace = 5;
const int32 iTypeHeight = 30;
const int32 iTitleLeftSpace = 16;
const int32 iTitleTopSpace = 45;
const int32 iTitleHeight = 36;
const int32 iContLeftSpace = 16;
const int32 iContTopSpace = 85;
const int32 iContHeight = 90;


const UINT KTimerId			  = 123470;	
const UINT kTimerPriod					  = 1000 * 15;


#ifdef _DEBUG
#define new DEBUG_NEW 
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
/////////////////////////////////////////////////////////////////////////////
CDlgPushMessage::CDlgPushMessage(CWnd* pParent /*=NULL*/)
: CDialog(CDlgPushMessage::IDD, pParent)
{
   
	m_pImgExit = NULL;
    m_iXButtonHovering = INVALID_ID;
    m_rectContent = CRect(0, 0, 0, 0);
}

CDlgPushMessage::~CDlgPushMessage()
{	
	DEL(m_pImgExit);
}

BEGIN_MESSAGE_MAP(CDlgPushMessage, CDialog)
    ON_WM_PAINT()
    ON_WM_TIMER()
    ON_WM_ACTIVATE()
    ON_WM_MOUSEMOVE()
    ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave) 
	ON_MESSAGE(WM_PRINTCLIENT, OnPrintClient) 
    ON_WM_LBUTTONDOWN()
    ON_WM_LBUTTONUP()	
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

void CDlgPushMessage::InitButtons()
{
	// ��Ϣ��Ŀ��
	int iDlgWidth = 300;		
	int iDlgHight = 200;

	m_pImgExit = Image::FromFile(L"image//exit.png");
	CString strVal = L"";

	int iWidth = m_pImgExit->GetWidth();
	int iHeight= m_pImgExit->GetHeight() / 3;
	int iLeftPos = iDlgWidth -iWidth -2;
	int iTopPos = 2;

	CRect rcBtn(iLeftPos, iTopPos, iLeftPos+iWidth, iTopPos+iHeight);

	CNCButton btnControl;
	btnControl.CreateButton(strVal, rcBtn, this, m_pImgExit, 3, ID_MY_BTN_CLOSE);
	btnControl.SetBtnBkgColor(RGB(44, 50, 55));
	m_mapButton[ID_MY_BTN_CLOSE] = btnControl;

	//// from left, top, right, and bottom
	//CRect(int l, int t, int r, int b) throw();

	// ����һ��85*25�İ�ť
	CRect rcOrder(iDlgWidth - 80, iDlgHight -25 -10, iDlgWidth - 10, iDlgHight - 10);
	m_rcOrder = rcOrder;

	CNCButton btnOrder;
	btnOrder.CreateButton(_T("�����µ�"), m_rcOrder, this, NULL, 0, ID_MY_BTN_ORDER);
	btnOrder.SetTextFrameColor(RGB(153,0,0), RGB(255,0,0), RGB(255,0,0));
	btnOrder.SetTextBkgColor(RGB(153,0,0), RGB(255,0,0), RGB(255,0,0));
	btnOrder.SetTextColor(RGB(255,255,255), RGB(255,255,255), RGB(255,255,255));
	m_mapButton[ID_MY_BTN_ORDER] = btnOrder;
}


void CDlgPushMessage::SetMsgTypeString(CString strMsgType)
{
	m_strMsgType = strMsgType;
}


int CDlgPushMessage::TButtonHitTest( CPoint point )
{
    map<int, CNCButton>::iterator iter;

    // ���������������а�ť
    for (iter = m_mapButton.begin(); iter!=m_mapButton.end(); ++iter)
    {
        CNCButton &btnControl = iter->second;

		if ((btnControl.GetControlId() == ID_MY_BTN_ORDER &&  0 != m_stMsg.m_StrMsgType.CollateNoCase(m_strMsgType)) ||
			(m_stMsg.m_StrMsgType.IsEmpty()&&btnControl.GetControlId() == ID_MY_BTN_ORDER))
		{
			continue;
		}

        // ��point�Ƿ����ѻ��Ƶİ�ť������
        if (btnControl.PtInButton(point)&& btnControl.GetCreate())
        {
            return btnControl.GetControlId();
        }
    }

    return INVALID_ID;
}

void CDlgPushMessage::OnPaint()
{
    CPaintDC dc(this);
    m_iXButtonHovering = INVALID_ID;
    Draw(dc);
}

void CDlgPushMessage::Draw( CDC& dc )
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

	
	CRect rtTitle(rcWindow), rtNoTitle(rcWindow);
	
	//	������
	{
		rtTitle.DeflateRect(1,1,1,1);
		Gdiplus::SolidBrush brTitle(Color(255,44,50,55));		
		rtTitle.bottom = rtTitle.top + iTitleHeight;
		graphics.FillRectangle(&brTitle, rtTitle.left, rtTitle.top, rtTitle.Width(), rtTitle.Height());
	}
	//	�ͻ���
	{
		Gdiplus::SolidBrush brTitle(Color(255,240,242,249));		
		rtNoTitle.DeflateRect(1,1,1,1);
		rtNoTitle.top += iTitleHeight ;
		graphics.FillRectangle(&brTitle, rtNoTitle.left, rtNoTitle.top, rtNoTitle.Width(), rtNoTitle.Height());
	}
    CRect rcPaint;
    dc.GetClipBox(&rcPaint);

    map<int, CNCButton>::iterator iter;
    CRect rcControl;

    // �������а�ť
    for (iter=m_mapButton.begin(); iter!=m_mapButton.end(); ++iter)
    {
        CNCButton &btnControl = iter->second;
        btnControl.GetRect(rcControl);

        // �жϵ�ǰ��ť�Ƿ���Ҫ�ػ�
		if (!rcPaint.IsRectEmpty() && !CRect().IntersectRect(&rcControl, rcPaint))
		{
			continue;
		}

		if ((btnControl.GetControlId() == ID_MY_BTN_ORDER &&  0 != m_stMsg.m_StrMsgType.CollateNoCase(m_strMsgType)) ||
			(m_stMsg.m_StrMsgType.IsEmpty()&&btnControl.GetControlId() == ID_MY_BTN_ORDER))
		{
			continue;
		}

        btnControl.DrawButton(&graphics);
    }

    DrawText(&graphics);
	
	Pen pen(Color(255, 83, 83, 83));	
	graphics.DrawRectangle(&pen, 0.0f, 0.0f, (float)(rcWindow.Width()-1) , (float)(rcWindow.Height()-1));
    dc.BitBlt(0, 0, rcWindow.Width(), rcWindow.Height(), &memDC, 0, 0, SRCCOPY);
    dc.SelectClipRgn(NULL);
    memDC.DeleteDC();
    bmp.DeleteObject();
}

void CDlgPushMessage::DoDataExchange( CDataExchange* pDX )
{
    CDialog::DoDataExchange(pDX);
}

BOOL CDlgPushMessage::OnInitDialog()
{
    UpdateData(FALSE);
    SetWindowPos(&wndTopMost,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);    
    InitButtons();

	//�˴���������ʾ�����ش�����Ч�ص㡣
	//����WS_EX_TOOLWINDOW��WS_EX_APPWINDOW����������Ի���Ϣ��ͻ����������С������ԭ�����г����������е��Ӵ����޷��������⣩
	//WS_WX_LAYERED�ǽ������ʾ���嶯�������г��ֱ�����top�߽���ְ�ɫϸ��
	//WS_EX_TOPMOST���Ǳ��������
	ModifyStyleEx(WS_EX_APPWINDOW, WS_EX_TOOLWINDOW|WS_EX_TOPMOST|WS_EX_LAYERED);
	SetLayeredWindowAttributes(RGB(255,0,255) ,0, LWA_COLORKEY);
    return TRUE;
}

void CDlgPushMessage::OnLButtonDown( UINT nFlags, CPoint point )
{
    int iButton = TButtonHitTest(point);
    if (INVALID_ID != iButton)
    {
        m_mapButton[iButton].LButtonDown();
    }
}

void CDlgPushMessage::OnMouseMove( UINT nFlags, CPoint point )
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
			m_mapButton[m_iXButtonHovering].MouseHover();
		}
	}

	// ��Ӧ WM_MOUSELEAVE��Ϣ
	TRACKMOUSEEVENT csTME;
	csTME.cbSize	= sizeof (csTME);
	csTME.dwFlags	= TME_LEAVE;
	csTME.hwndTrack = m_hWnd ;		// ָ��Ҫ׷�ٵĴ��� 
	if(::_TrackMouseEvent (&csTME))	// ����Windows��WM_MOUSELEAVE�¼�֧�� 
	{
		KillTimer(KTimerId);
	}

	CDialog::OnMouseMove(nFlags, point);
}

LRESULT CDlgPushMessage::OnMouseLeave( WPARAM, LPARAM )
{
    SetTimer(KTimerId, kTimerPriod, NULL);

    if (INVALID_ID != m_iXButtonHovering)
    {
        m_mapButton[m_iXButtonHovering].MouseLeave();
        m_iXButtonHovering = INVALID_ID;
    }

    return 0;    
}

void CDlgPushMessage::OnLButtonUp( UINT nFlags, CPoint point )
{
    if(m_rectContent.Width() > 0 && m_rectContent.Height() > 0)
    {
        if(m_rectContent.PtInRect(point) && !m_stMsg.m_StrContentUrl.IsEmpty())
        {
            ShellExecute(0, L"open", m_stMsg.m_StrContentUrl, NULL, NULL, SW_SHOWNORMAL);
        }
    }

    int iButton = TButtonHitTest(point);
    if (INVALID_ID != iButton)
    {
        m_mapButton[iButton].LButtonUp();
    }

    CDialog::OnLButtonUp(nFlags, point);
}

void CDlgPushMessage::DrawText( Graphics *pGraphics )
{
    CRect rtClient;
    GetClientRect(&rtClient);
    int iClientWidth = rtClient.Width();
    int iClientHeight = rtClient.Height();
    if((0 == iClientWidth) || (0 == iClientHeight))
    {
        return;
    }

    //��ʽ
    StringFormat strFormat;
    strFormat.SetAlignment(StringAlignmentNear);
    strFormat.SetLineAlignment(StringAlignmentCenter);

    //��ɫ
    SolidBrush brush((ARGB)Color::White);

    // ������Ϣ����
    {
        CString strFontNameType = gFontFactory.GetExistFontName(L"����");
        Gdiplus::FontFamily fontFamilyType(strFontNameType);
        Gdiplus::Font fontType(&fontFamilyType, 10, FontStyleRegular, UnitPoint); 

        CString StrMsgType = m_stMsg.m_StrMsgType;

		if (StrMsgType.IsEmpty())
		{
			StrMsgType = L"��Ϣ����";
		}

		RectF rcType;
		rcType.X = (REAL)iTypeLeftSpace;
		rcType.Y = (REAL)iTypeTopSpace;
		rcType.Width = (REAL)(iClientWidth - iTypeLeftSpace);
		rcType.Height = (REAL)iTypeHeight;

		brush.SetColor(Color::Color(255,255,255));
		pGraphics->DrawString(StrMsgType, StrMsgType.GetLength(), &fontType, rcType, &strFormat, &brush);
    }

    // ������Ϣ����
    {
        CString strFontNameTitle = gFontFactory.GetExistFontName(L"����");
        Gdiplus::FontFamily fontFamilyTitle(strFontNameTitle);
        Gdiplus::Font fontTitle(&fontFamilyTitle, 11, FontStyleRegular|FontStyleBold, UnitPoint); 

        CString StrMsgTitle = m_stMsg.m_StrTitle;
        if (!StrMsgTitle.IsEmpty())
        {
            RectF rcTitle;
            rcTitle.X = (REAL)iTitleLeftSpace;
            rcTitle.Y = (REAL)iTitleTopSpace;
            rcTitle.Width = (REAL)(iClientWidth - iTitleLeftSpace);
            rcTitle.Height = (REAL)iTitleHeight;

            brush.SetColor(Color::Color(0, 0, 0));
            pGraphics->DrawString(StrMsgTitle, StrMsgTitle.GetLength(), &fontTitle, rcTitle, &strFormat, &brush);
        }
    }

    // ������Ϣ����
    {
        strFormat.SetLineAlignment(StringAlignmentNear);

        CString strFontNameCont = gFontFactory.GetExistFontName(L"����");
        Gdiplus::FontFamily fontFamilyCont(strFontNameCont);
        Gdiplus::Font fontCont(&fontFamilyCont, 10, FontStyleRegular, UnitPoint); 

        CString StrMsgCont = m_stMsg.m_StrContent;
        if (!StrMsgCont.IsEmpty())
        {
            RectF rcCont;
            rcCont.X = (REAL)iContLeftSpace;
			if(m_stMsg.m_StrTitle.IsEmpty())
			{
                rcCont.Y = (REAL)iTitleTopSpace;
			}
			else
			{
				rcCont.Y = (REAL)iContTopSpace;
			}
            rcCont.Width = (REAL)(iClientWidth - iContLeftSpace);
            rcCont.Height = (REAL)iContHeight;

            RectF rcBound;
            PointF point;
            point.X = rcCont.X;
            point.Y = rcCont.Y;
            pGraphics->MeasureString(StrMsgCont, StrMsgCont.GetLength(), &fontCont, point, &strFormat, &rcBound);
            bool bSuilt = ((int(rcBound.Width)) % (iClientWidth - iContLeftSpace ) == 0);
            int iLineCount = bSuilt?((int)rcBound.Width/(iClientWidth - iContLeftSpace )):((int)rcBound.Width/(iClientWidth - iContLeftSpace ) + 1);

            m_rectContent.left = iContLeftSpace;
            m_rectContent.right = iClientWidth;
            m_rectContent.top = iContTopSpace;
            m_rectContent.bottom = long(iContTopSpace + iLineCount * rcBound.Height);

            brush.SetColor(Color::Color(131,135,144));
            pGraphics->DrawString(StrMsgCont, StrMsgCont.GetLength(), &fontCont, rcCont, &strFormat, &brush);
        }
    }
}

void CDlgPushMessage::InvalidateMsg(T_pushMsg stMsg)
{
    m_stMsg.m_StrMsgType = stMsg.m_StrMsgType;
    m_stMsg.m_StrTitle = stMsg.m_StrTitle;
    m_stMsg.m_StrContent = stMsg.m_StrContent;
    m_stMsg.m_StrContentUrl = stMsg.m_StrContentUrl;
    Invalidate(FALSE);
}

void CDlgPushMessage::SetMsg( T_pushMsg stMsg )
{
    m_stMsg.m_StrMsgType = stMsg.m_StrMsgType;
    m_stMsg.m_StrTitle = stMsg.m_StrTitle;
    m_stMsg.m_StrContent = stMsg.m_StrContent;
    m_stMsg.m_StrContentUrl = stMsg.m_StrContentUrl;
}

BOOL CDlgPushMessage::OnCommand( WPARAM wParam, LPARAM lParam )
{
	int32 iID = (int32)wParam;
	if (iID == ID_MY_BTN_CLOSE)
	{
		HideDialog();
		OnOK();
	}

	if (iID == ID_MY_BTN_ORDER)
	{
		CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
		if (pMainFrame && pMainFrame->m_hWnd)
		{
			::PostMessage(pMainFrame->m_hWnd, UM_JumpToTradingSoftware,0,0);

		}
		HideDialog();
		OnOK();	
	}

    return CDialog::OnCommand(wParam, lParam);
}


void CDlgPushMessage::OnActivate( UINT nState, CWnd* pWndOther, BOOL bMinimized )
{
    CDialog::OnActivate(nState, pWndOther, bMinimized);  
}

void CDlgPushMessage::OnTimer( UINT nIDEvent )
{
    if(nIDEvent == KTimerId)
    {
        HideDialog();
        KillTimer(KTimerId);
    }
}

LRESULT CDlgPushMessage::OnPrintClient(WPARAM wParam, LPARAM lParam)
{
	//����Ϣ�Ǵ�����Ч�ڶ��������е��ã��൱��OnPaint,�ڴ��嶯����Ч��OnPaint��Ч��
	if(wParam)
	{
		CDC dc;
		dc.Attach((HDC)wParam);
		Draw(dc);
		dc.Detach();
	}
	return 0;
}

void CDlgPushMessage::HideDialog()
{
	 //ModifyStyleEx(WS_EX_LAYERED, 0);//�˴��������㼶���ڣ����嶯����Ч��ʧ����
	 AnimateWindow(1500, AW_VER_POSITIVE|AW_HIDE|AW_SLIDE);
}
void CDlgPushMessage::ShowDialog()
{   
	//ModifyStyleEx(0, WS_EX_LAYERED);//�˴��������ô���������Ч����top�߽��ɫ��������
	//SetLayeredWindowAttributes(RGB(255,0,255) ,0, LWA_COLORKEY);
	AnimateWindow(1500, AW_VER_NEGATIVE|AW_SLIDE|AW_ACTIVATE);
	SetTimer(KTimerId, kTimerPriod, NULL);
}
