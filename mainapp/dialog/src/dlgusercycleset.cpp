// dlgusercycleset.cpp : implementation file
//

#include "stdafx.h"
#include "dlgusercycleset.h"
#include "IoViewShare.h"
#include "IoViewKLine.h"
#include "IoViewDetail.h"
#include "IoViewKLineArbitrage.h"
#include "FontFactory.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace
{
#define INVALID_ID -1
#define ID_EDIT_PERIOD                  4900
#define ID_PERIOD_1_MINUTE              5000
#define ID_PERIOD_3_MINUTE              5001
#define ID_PERIOD_5_MINUTE              5002
#define ID_PERIOD_10_MINUTE             5003
#define ID_PERIOD_15_MINUTE             5004
#define ID_PERIOD_30_MINUTE             5005
#define ID_PERIOD_60_MINUTE             5006
#define ID_PERIOD_120_MINUTE            5007
#define ID_PERIOD_180_MINUTE            5008
#define ID_PERIOD_240_MINUTE            5009
#define ID_PERIOD_DAY_LINE              5010
#define ID_PERIOD_WEEK_LINE             5011
#define ID_PERIOD_MONTH_LINE            5012
#define ID_PERIOD_YEAR_LINE             5013
#define ID_PERIOD_OK                    5020
}

/////////////////////////////////////////////////////////////////////////////
// CDlgUserCycleSet dialog
//#define CHECK_ON_MINUTE      20001
//#define CHECK_FIVE_MINUTE    20002

CDlgUserCycleSet::CDlgUserCycleSet(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgUserCycleSet::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgUserCycleSet)
	//m_StrPre = _T("");
	m_StrAft = _T("");
	m_uiUserSet = 0;
	//}}AFX_DATA_INIT
	m_pParentIoView = NULL;
    m_uiOldUserSet = 0;
    m_iXButtonHovering  = INVALID_ID;
	m_eNodeInterval     = ENTIMinute;
	m_pImgBk            = NULL;
	m_pImgCheckPeriod   = NULL;
	m_pImgCheckPeriodOK = NULL;
}

CDlgUserCycleSet::~CDlgUserCycleSet()
{
	DEL(m_pImgBk);
	DEL(m_pImgCheckPeriod);
	DEL(m_pImgCheckPeriodOK);
}

void CDlgUserCycleSet::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgUserCycleSet)
	//DDX_Text(pDX, IDC_STATIC1, m_StrPre);
	DDX_Text(pDX, IDC_STATIC2, m_StrAft);
    //DDX_Control(pDX, ID_EDIT_PERIOD, m_ePeriod);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDlgUserCycleSet, CDialog)
	//{{AFX_MSG_MAP(CDlgUserCycleSet)
	ON_WM_CLOSE()
    ON_WM_PAINT()
    ON_WM_CTLCOLOR()
    ON_WM_ACTIVATE()
    ON_WM_MOUSEMOVE()
    ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave) 
    ON_WM_LBUTTONDOWN()
    ON_WM_LBUTTONUP()
	ON_COMMAND(IDC_CONFIRM, OnButtonConfirm)
    ON_EN_KILLFOCUS(ID_EDIT_PERIOD, OnEnKillFocusEditPeriod)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgUserCycleSet message handlers

BOOL CDlgUserCycleSet::PreTranslateMessage(MSG* pMsg) 
{	
	if ( WM_KEYDOWN == pMsg->message)
	{
		if ( VK_ESCAPE == pMsg->wParam )
		{
			SendMessage(WM_CLOSE, 0, 0);
			return TRUE;
		}
        else if(VK_RETURN == pMsg->wParam)
        {
            OnOK();
        }
	}

	return CDialog::PreTranslateMessage(pMsg);
}

BOOL CDlgUserCycleSet::OnInitDialog() 
{
	CDialog::OnInitDialog();
    InitCheckButton();
    InitKlineCheckPeriod();
	
	// TODO: Add extra initialization here
	if ( ENTIDayUser == m_eNodeInterval)
	{ 
        for(int i = ID_PERIOD_1_MINUTE; i<= ID_PERIOD_YEAR_LINE; i++)
        {
            if(i == ID_PERIOD_DAY_LINE || i == ID_PERIOD_WEEK_LINE || 
               i == ID_PERIOD_MONTH_LINE || i == ID_PERIOD_YEAR_LINE)
            {
                continue;
            }

            //GetDlgItem(i)->EnableWindow(FALSE);
        }
        m_StrAft = L"天 ( 1 - 365)";
	}
    else
    {
        m_StrAft = L"分钟(1 - 720)";
    }

	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgUserCycleSet::InitMapCheckItem()
{
    m_mapCheckItem.clear();
    m_mapCheckItem[PT_1_MINUTE] = L"1分钟";
    m_mapCheckItem[PT_3_MINUTE] = L"3分钟";
    m_mapCheckItem[PT_5_MINUTE] = L"5分钟";
    m_mapCheckItem[PT_10_MINUTE] = L"10分钟";
    m_mapCheckItem[PT_15_MINUTE] = L"15分钟";
    m_mapCheckItem[PT_30_MINUTE] = L"30分钟";
    m_mapCheckItem[PT_60_MINUTE] = L"60分钟";
    m_mapCheckItem[PT_120_MINUTE] = L"120分钟";
    m_mapCheckItem[PT_180_MINUTE] = L"180分钟";
    m_mapCheckItem[PT_240_MINUTE] = L"240分钟";
    m_mapCheckItem[PT_DAY_LINE] = L"日线";
    m_mapCheckItem[PT_WEEK_LINE] = L"周线";
    m_mapCheckItem[PT_MONTH_LINE] = L"月线";
    m_mapCheckItem[PT_YEAR_LINE] = L"年线";
}

void CDlgUserCycleSet::InitCheckButton()
{
    InitMapCheckItem();
    CRect rect(5, 9, 45, 33);
    m_ePeriod.Create(ES_MULTILINE|ES_NUMBER|SS_CENTER|WS_VISIBLE|WS_BORDER, rect, this, ID_EDIT_PERIOD);
    LOGFONT lgFont = {0};
    lgFont.lfHeight =20;
    lgFont.lfWidth = 0;
    lgFont.lfEscapement = 0;
    lgFont.lfOrientation = 0;
    lgFont.lfWeight = FW_BOLD;
    lgFont.lfItalic = 0;
    lgFont.lfUnderline = 0;
    lgFont.lfStrikeOut = 0;
    lgFont.lfCharSet = ANSI_CHARSET;
    lgFont.lfOutPrecision = OUT_DEFAULT_PRECIS;
    lgFont.lfClipPrecision = CLIP_DEFAULT_PRECIS;
    lgFont.lfQuality = PROOF_QUALITY;
    lgFont.lfPitchAndFamily = VARIABLE_PITCH | FF_ROMAN;
    _tcscpy(lgFont.lfFaceName, L"宋体");///

    LOGFONT	lgFontEditMid = lgFont;
    lgFontEditMid.lfHeight = 15;

    m_fontEditSmall.CreateFontIndirect(&lgFontEditMid);
    m_ePeriod.SetFont(&m_fontEditSmall);

    int iWidth = 0;
    int iHeight = 0;
    int iLeftPos = 0;	
    int iTopPos = 43;

    m_pImgCheckPeriod = Image::FromFile(L"image//checkPeriod.png");
    if(NULL != m_pImgCheckPeriod && m_pImgCheckPeriod->GetLastStatus() == Ok)
    {
        iWidth = m_pImgCheckPeriod->GetWidth();
        iHeight = m_pImgCheckPeriod->GetHeight() / 3;
        for(int Index = 0; Index < m_mapCheckItem.size(); Index++)
        {
            int iX = iLeftPos;
            int iY = iTopPos + Index * (iHeight - 1);

            CRect rcBtn(iX, iY, iX+iWidth, iY+iHeight);
            AddCheckButton(&rcBtn, m_pImgCheckPeriod, 3, ID_PERIOD_1_MINUTE + Index, m_mapCheckItem[Index], L"");
        }
    }

    m_pImgCheckPeriodOK = Image::FromFile(L"image//checkPeriodOK.png");
    if(NULL != m_pImgCheckPeriodOK && m_pImgCheckPeriodOK->GetLastStatus() == Ok)
    {
        iWidth = m_pImgCheckPeriodOK->GetWidth();
        iHeight = m_pImgCheckPeriodOK->GetHeight() / 3;

        iLeftPos = 165;
        iTopPos = 9;
        CRect rcBtn(iLeftPos, iTopPos, iLeftPos+iWidth, iTopPos+iHeight);
        AddCheckButton(&rcBtn, m_pImgCheckPeriodOK, 3, ID_PERIOD_OK, L"确定", L"");
    }
}

void CDlgUserCycleSet::DrawCheckButton( CDC& dc )
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

    // 工具栏背景
    m_pImgBk = Image::FromFile(L"image//checkPeriodBK.png");
    if(NULL != m_pImgBk && m_pImgBk->GetLastStatus() == Ok)
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

    // 遍历所有按钮
    for (iter=m_mapCheckBtn.begin(); iter!=m_mapCheckBtn.end(); ++iter)
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

void CDlgUserCycleSet::OnPaint()
{
    CPaintDC dc(this);

    m_iXButtonHovering = INVALID_ID;
    DrawCheckButton(dc);
}

void CDlgUserCycleSet::OnMouseMove( UINT nFlags, CPoint point )
{
    int iButton = TCheckButtonHitTest(point);

    if (iButton != m_iXButtonHovering)
    {
        if (INVALID_ID != m_iXButtonHovering)
        {
            m_mapCheckBtn[m_iXButtonHovering].MouseLeave();
            m_iXButtonHovering = INVALID_ID;
        }

        if (INVALID_ID != iButton)
        {	
            m_iXButtonHovering = iButton;
            m_mapCheckBtn[m_iXButtonHovering].MouseHover();
        }
    }

    // 响应 WM_MOUSELEAVE消息
    TRACKMOUSEEVENT csTME;
    csTME.cbSize	= sizeof (csTME);
    csTME.dwFlags	= TME_LEAVE;
    csTME.hwndTrack = m_hWnd ;		// 指定要追踪的窗口 
    ::_TrackMouseEvent (&csTME);	// 开启Windows的WM_MOUSELEAVE事件支持 

    CWnd::OnMouseMove(nFlags, point);
}

LRESULT CDlgUserCycleSet::OnMouseLeave( WPARAM, LPARAM )
{
    if (INVALID_ID != m_iXButtonHovering)
    {
        m_mapCheckBtn[m_iXButtonHovering].MouseLeave();
        m_iXButtonHovering = INVALID_ID;
    }
    return 0;
}

void CDlgUserCycleSet::OnLButtonDown( UINT nFlags, CPoint point )
{
    int iButton = 0;
    iButton = TCheckButtonHitTest(point);
    if (INVALID_ID != iButton)
    {
        m_mapCheckBtn[iButton].SetCheck(TRUE);

        map<int, CNCButton>::iterator iter;
        for (iter=m_mapCheckBtn.begin(); iter!=m_mapCheckBtn.end(); ++iter)
        {
            CNCButton &btn = iter->second;
            if (iButton != iter->first && btn.GetCheck())
            {
                btn.SetCheckStatus(FALSE);
            }
        }

        SetPeriodInfo(iButton);
    }
}

void CDlgUserCycleSet::OnLButtonUp( UINT nFlags, CPoint point )
{
    int iButton = TCheckButtonHitTest(point);
    if (INVALID_ID != iButton)
    {
        m_mapCheckBtn[iButton].LButtonUp();
    }

    CDialog::OnLButtonUp(nFlags, point);
}

int CDlgUserCycleSet::TCheckButtonHitTest( CPoint point )
{
    map<int, CNCButton>::iterator iter;

    // 遍历工具栏上所有按钮
    for (iter=m_mapCheckBtn.begin(); iter!=m_mapCheckBtn.end(); ++iter)
    {
        CNCButton &btnControl = iter->second;

        // 点point是否在已绘制的按钮区域内
        if (btnControl.PtInButton(point) && btnControl.GetCreate())
        {
            return btnControl.GetControlId();
        }
    }

    return INVALID_ID;
}

void CDlgUserCycleSet::AddCheckButton( LPRECT lpRect, Image *pImage, UINT nCount/*=3*/, UINT nID/*=0*/, LPCTSTR lpszCaption/*=NULL*/, CString strHotImg/*=L""*/ )
{
    ASSERT(pImage);
    CNCButton btnControl;
    btnControl.CreateButton(lpszCaption, lpRect, this, pImage, nCount, nID);
    btnControl.SetTextColor(RGB(116,89,28), RGB(116,89,28), RGB(116,89,28));

    CNCButton::T_NcFont m_Font;
    m_Font.m_StrName = gFontFactory.GetExistFontName(L"微软雅黑");
    m_Font.m_Size	 = 10;
    m_Font.m_iStyle	 = FontStyleRegular;	
    btnControl.SetFont(m_Font);

    m_mapCheckBtn[nID] = btnControl;
}

void CDlgUserCycleSet::InitKlineCheckPeriod()
{
    CMainFrame* pMain = (CMainFrame*)AfxGetMainWnd();
    CString StrSet;
    StrSet.Format(L"%d", pMain->m_LastTimeSet);
    if(NULL != pMain)
    {
        UINT timeSet = pMain->m_LastTimeSet;
        UINT uID = (UINT)-1;
        if(timeSet > 0)
        {
            switch(timeSet)
            {
            case 1:
                uID = ID_PERIOD_1_MINUTE;
                m_ePeriod.ShowWindow(SW_SHOW);
                m_uiUserSet = pMain->m_LastTimeSet;
                break;

            case 3:
                uID = ID_PERIOD_3_MINUTE;
                m_ePeriod.ShowWindow(SW_SHOW);
                m_uiUserSet = pMain->m_LastTimeSet;
                break;

            case 5:
                uID = ID_PERIOD_5_MINUTE;
                m_ePeriod.ShowWindow(SW_SHOW);
                m_uiUserSet = pMain->m_LastTimeSet;
                break;

            case 10:
                uID = ID_PERIOD_10_MINUTE;
                m_ePeriod.ShowWindow(SW_SHOW);
                m_uiUserSet = pMain->m_LastTimeSet;
                break;

            case 15:
                uID = ID_PERIOD_15_MINUTE;
                m_ePeriod.ShowWindow(SW_SHOW);
                m_uiUserSet = pMain->m_LastTimeSet;
                break;

            case 30:
                uID = ID_PERIOD_30_MINUTE;
                m_ePeriod.ShowWindow(SW_SHOW);
                m_uiUserSet = pMain->m_LastTimeSet;
                break;

            case 60:
                uID = ID_PERIOD_60_MINUTE;
                m_ePeriod.ShowWindow(SW_SHOW);
                m_uiUserSet = pMain->m_LastTimeSet;
                break;

            case 120:
                uID = ID_PERIOD_120_MINUTE;
                m_ePeriod.ShowWindow(SW_SHOW);
                m_uiUserSet = pMain->m_LastTimeSet;
                break;

            case 180:
                uID = ID_PERIOD_180_MINUTE;
                m_ePeriod.ShowWindow(SW_SHOW);
                m_uiUserSet = pMain->m_LastTimeSet;
                break;

            case 240:
                uID = ID_PERIOD_240_MINUTE;
                m_ePeriod.ShowWindow(SW_SHOW);
                m_uiUserSet = pMain->m_LastTimeSet;
                break;

            case 1440:
                uID = ID_PERIOD_DAY_LINE;
                m_StrAft = L"本日";
                m_ePeriod.ShowWindow(SW_HIDE);
                break;

            case 1440*7:
                uID = ID_PERIOD_WEEK_LINE;
                m_StrAft = L"本周";
                m_ePeriod.ShowWindow(SW_HIDE);
                break;

            case 1440*30:
                uID = ID_PERIOD_MONTH_LINE;
                m_StrAft = L"本月";
                m_ePeriod.ShowWindow(SW_HIDE);
                break;

            case 1440*365:
                uID = ID_PERIOD_YEAR_LINE;
                m_StrAft = L"本年";
                m_ePeriod.ShowWindow(SW_HIDE);
                break;

            default:
                {
                    break;
                }
            }
        }

        SetChecPeriodBtnStatus(uID);

        if(pMain->m_LastTimeSet < 1440)
        {
            CString StrUserSet;
            StrUserSet.Format(_T("%d"), m_uiUserSet);
            GetDlgItem(ID_EDIT_PERIOD)->SetWindowText(StrUserSet);
        }


        //UpdateData(FALSE);
    }
}

void CDlgUserCycleSet::OnButtonConfirm()
{
	OnOK();
}

void CDlgUserCycleSet::OnClose() 
{
	CDialog::OnClose();
}

void CDlgUserCycleSet::OnOK()
{
	//UINT uiOldUserSet = m_uiUserSet;

    CMainFrame* pMain = (CMainFrame*)AfxGetMainWnd();

	//if(!UpdateData(TRUE))
	//{
	//	return;
	//}

    CString  strSet;
    GetDlgItem(ID_EDIT_PERIOD)->GetWindowText(strSet);
    if(GetDlgItem(ID_EDIT_PERIOD)->IsWindowVisible())
    {
        m_uiUserSet = _ttoi(strSet.GetBuffer());
    }
    
	UINT uiUserSetBk = m_uiUserSet;
	//
	ASSERT( NULL != m_pParentIoView );

	if ( ENTIDayUser == m_eNodeInterval )
	{
		if(uiUserSetBk < USERSET_DAY_MIN)
		{
			m_uiUserSet = USERSET_DAY_MIN;
		}
		else if(uiUserSetBk > USERSET_DAY_MAX)
		{
			m_uiUserSet = USERSET_DAY_MAX;
		}

		if(pMain->m_LastTimeSet == m_uiUserSet)
		{
			CDialog::OnOK();
			return;
		}
	}
	else if (ENTIMinuteUser == m_eNodeInterval)
	{
		if(uiUserSetBk < USERSET_MINUET_MIN)
		{
			m_uiUserSet = USERSET_MINUET_MIN;
		}
		else if(uiUserSetBk > USERSET_MINUET_MAX)
		{
			m_uiUserSet = USERSET_MINUET_MAX;
		}

		if(pMain->m_LastTimeSet == m_uiUserSet)
		{
			CDialog::OnOK();
			return;
		}
	}

    //m_uiOldUserSet = m_uiUserSet;
    
    if(pMain)
    {
        pMain->m_LastTimeSet = m_uiUserSet;
    }

	if ( m_pParentIoView->IsKindOf(RUNTIME_CLASS(CIoViewDetail)))
	{
		CIoViewDetail* pDetail = (CIoViewDetail*)m_pParentIoView;
		pDetail->OnUserCircleChanged(m_eNodeInterval, m_uiUserSet);

		CDialog::OnOK();
	}
	else if ( m_pParentIoView->IsKindOf(RUNTIME_CLASS(CIoViewKLine)))
	{
		CIoViewKLine* pKLine = (CIoViewKLine*)m_pParentIoView;
		pKLine->OnUserCircleChanged(m_eNodeInterval, m_uiUserSet);

        pKLine->UpdateKeline(m_eNodeInterval);
		CDialog::OnOK();
	}
	else if ( m_pParentIoView->IsKindOf(RUNTIME_CLASS(CIoViewKLineArbitrage)))
	{
		CIoViewKLineArbitrage* pKLineArbitrage = (CIoViewKLineArbitrage*)m_pParentIoView;
		pKLineArbitrage->OnUserCircleChanged(m_eNodeInterval, m_uiUserSet);
        pKLineArbitrage->UpdateKelineArbitrage(m_eNodeInterval);

		CDialog::OnOK();
	}
}

void CDlgUserCycleSet::SetPeriodInfo(UINT nID)
{
    switch(nID)
    {
    case ID_PERIOD_1_MINUTE:
        m_uiUserSet = 1;
        m_StrAft = L"分钟(1 - 720)";
        m_eNodeInterval = ENTIMinuteUser;
        m_ePeriod.ShowWindow(SW_NORMAL);
        break;

    case ID_PERIOD_3_MINUTE:
        m_uiUserSet = 3;
        m_StrAft = L"分钟(1 - 720)";
        m_eNodeInterval = ENTIMinuteUser;
        m_ePeriod.ShowWindow(SW_NORMAL);
        break;

    case ID_PERIOD_5_MINUTE:
        m_uiUserSet = 5;
        m_StrAft = L"分钟(1 - 720)";
        m_eNodeInterval = ENTIMinuteUser;
        m_ePeriod.ShowWindow(SW_NORMAL);
        break;

    case ID_PERIOD_10_MINUTE:
        m_uiUserSet = 10;
        m_StrAft = L"分钟(1 - 720)";
        m_eNodeInterval = ENTIMinuteUser;
        m_ePeriod.ShowWindow(SW_NORMAL);
        break;

    case ID_PERIOD_15_MINUTE:
        m_uiUserSet = 15;
        m_StrAft = L"分钟(1 - 720)";
        m_eNodeInterval = ENTIMinuteUser;
        m_ePeriod.ShowWindow(SW_NORMAL);
        break;

    case ID_PERIOD_30_MINUTE:
        m_uiUserSet = 30;
        m_StrAft = L"分钟(1 - 720)";
        m_eNodeInterval = ENTIMinuteUser;
        m_ePeriod.ShowWindow(SW_NORMAL);
        break;

    case ID_PERIOD_60_MINUTE:
        m_uiUserSet = 60;
        m_StrAft = L"分钟(1 - 720)";
        m_eNodeInterval = ENTIMinuteUser;
        m_ePeriod.ShowWindow(SW_NORMAL);
        break;

    case ID_PERIOD_120_MINUTE:
        m_uiUserSet = 120;
        m_StrAft = L"分钟(1 - 720)";
        m_eNodeInterval = ENTIMinuteUser;
        m_ePeriod.ShowWindow(SW_NORMAL);
        break;

    case ID_PERIOD_180_MINUTE:
        m_uiUserSet = 180;
        m_StrAft = L"分钟(1 - 720)";
        m_eNodeInterval = ENTIMinuteUser;
        m_ePeriod.ShowWindow(SW_NORMAL);
        break;

    case ID_PERIOD_240_MINUTE:
        m_uiUserSet = 240;
        m_StrAft = L"分钟(1 - 720)";
        m_eNodeInterval = ENTIMinuteUser;
        m_ePeriod.ShowWindow(SW_NORMAL);
        break;

    case ID_PERIOD_DAY_LINE:
        m_uiUserSet =24*60;
        m_StrAft = L"本日";
        m_eNodeInterval = ENTIDay;
        m_ePeriod.ShowWindow(SW_HIDE);
        break;

    case ID_PERIOD_WEEK_LINE:
        m_uiUserSet = 24*60* 7;
        m_StrAft = L"本周";
        m_eNodeInterval = ENTIWeek;
        m_ePeriod.ShowWindow(SW_HIDE);
        break;

    case ID_PERIOD_MONTH_LINE:
        m_uiUserSet = 24*60* 30;
        m_StrAft = L"本月";
        m_eNodeInterval = ENTIMonth;
        m_ePeriod.ShowWindow(SW_HIDE);
        break;

    case ID_PERIOD_YEAR_LINE:
        m_uiUserSet = 24*60* 365;
        m_StrAft = L"本年";
        m_eNodeInterval = ENTIYear;
        m_ePeriod.ShowWindow(SW_HIDE);
        break;

    default:
        {
        }
        break;
    }

    if(m_ePeriod.IsWindowVisible())
    {
        CString StrUserSet;
        StrUserSet.Format(_T("%d"), m_uiUserSet);
        GetDlgItem(ID_EDIT_PERIOD)->SetWindowText(StrUserSet);
    }

    UpdateData(FALSE);
}

void CDlgUserCycleSet::SetChecPeriodBtnStatus(UINT nID)
{
    map<int, CNCButton>::iterator iter;
    for (iter=m_mapCheckBtn.begin(); iter!=m_mapCheckBtn.end(); ++iter)
    {
        CNCButton &btn = iter->second;
        if (btn.GetCheck())
        {
            btn.SetCheckStatus(FALSE);
        }
    }

    if(nID >= ID_PERIOD_1_MINUTE || nID <= ID_PERIOD_YEAR_LINE)
    {
        m_mapCheckBtn[nID].SetCheckStatus(TRUE);
    }
}


void CDlgUserCycleSet::SetInitialParam( E_NodeTimeInterval eNodeInterval, UINT uiUserSet )
{
    m_eNodeInterval = eNodeInterval; 
    //m_uiUserSet = uiUserSet;

    if ( ENTIDayUser == m_eNodeInterval)
    { 
        m_StrAft = L"天 ( 1 - 365)";
    }
    else
    {
        m_StrAft = L"分钟(1 - 720)";
    }
    InitKlineCheckPeriod();
    UpdateData(FALSE);
}

void CDlgUserCycleSet::OnActivate( UINT nState, CWnd* pWndOther, BOOL bMinimized )
{
    CDialog::OnActivate(nState, pWndOther, bMinimized);  
    if (WA_INACTIVE == nState)  
    {  
        PostMessage(WM_CLOSE, NULL, NULL);  
    }  
}

HBRUSH CDlgUserCycleSet::OnCtlColor( CDC* pDC, CWnd* pWnd, UINT nCtlColor )
{
    HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

    if(nCtlColor ==  CTLCOLOR_STATIC)
    {
        pDC->SetBkMode(TRANSPARENT);
        pDC->SetTextColor(RGB(116,89,28));	// 静态文本框文字的颜色 
        static HBRUSH hbrr = ::CreateSolidBrush(RGB(250, 247, 234));
        //pDC->SetBkColor(RGB(236, 239, 243));
        return hbrr;

    }
    return hbr;
}

BOOL CDlgUserCycleSet::OnCommand( WPARAM wParam, LPARAM lParam )
{
    int32 iID = (int32)wParam;

    std::map<int, CNCButton>::iterator itHome = m_mapCheckBtn.find(iID);
    if (m_mapCheckBtn.end() != itHome)
    {
        if(ID_PERIOD_OK == iID)
        {
            OnOK();
        }
    }

    return CDialog::OnCommand(wParam, lParam);
}

void CDlgUserCycleSet::OnEnKillFocusEditPeriod()
{
    CString str;
    m_ePeriod.GetWindowText(str);
    m_uiUserSet = _ttoi(str);
}
