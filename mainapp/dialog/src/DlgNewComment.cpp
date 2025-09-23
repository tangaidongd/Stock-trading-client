#include "stdafx.h"
#include "DlgNewComment.h"
#include <WindowsX.h>


/////////////////////////////////////////////////////////////////////////////
CDlgNewCommentIE::CDlgNewCommentIE(CWnd* pParent /*=NULL*/)
: CDialogEx(CDlgNewCommentIE::IDD, pParent)
{
    m_pWndCef = NULL;
}

CDlgNewCommentIE::~CDlgNewCommentIE()
{
    if(m_pWndCef != NULL)
    {
        delete m_pWndCef;
        m_pWndCef = NULL;
    }
}

void CDlgNewCommentIE::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CDlgNewCommentIE, CDialogEx)
    ON_WM_PAINT()
    ON_WM_ERASEBKGND()
    ON_WM_SIZE()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
BOOL CDlgNewCommentIE::OnInitDialog() 
{
    CDialogEx::OnInitDialog();

    SetWindowText(m_strTitle);

    CRect rect(0, 0 , 840, 610);
    this->MoveWindow(&rect);
    this->CenterWindow();

    return TRUE;  // return TRUE unless you set the focus to a control
    // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgNewCommentIE::SetTitleUrl( const CString &StrTitle, const CString &StrUrl)
{
    m_strTitle = StrTitle;
    m_strUrl = StrUrl;

    if ( NULL != m_hWnd )
    {
        SetWindowText(StrTitle);
    }
}

void CDlgNewCommentIE::OnSize( UINT nType, int cx, int cy )
{
    CDialogEx::OnSize(nType, cx, cy);

	if(NULL != m_pWndCef)
	{
		if(IsRestored(m_hWnd) || IsZoomed())
		{
			m_pWndCef->MoveWindow(0, 0, cx, cy);
		}
		else
		{
			// 减去标题栏的高度
			m_pWndCef->MoveWindow(0, 0, cx, cy - 33);
		}
	}
}

void CDlgNewCommentIE::OnOK()
{
    // 检查一下值

    CDialogEx::OnOK();
}

void CDlgNewCommentIE::ShowDlgIEWithSize(const CString &StrTitle, const CString &StrUrl, const CRect& rect, int bFlag /* = 0 */)
{
    if ( 1 == bFlag )
    {
        if ( NULL == m_hWnd )
        {
            return;
        }
        ::SendMessage(m_hWnd, WM_SYSCOMMAND, SC_CLOSE, 0);

        return;
    }

    CRect rtShow = rect;
    if (rect.IsRectEmpty())
    {
        rtShow = CRect(0,0,800,600);		
    }

    if (NULL ==  m_hWnd )
    {
        Create(IDD, AfxGetMainWnd());
    }

    SetTitleUrl(StrTitle, StrUrl);
    ShowWeb(rtShow);	// 显示浏览器内容
	MoveWindow(&rtShow);
	CenterWindow();	 
    ShowWindow(SW_NORMAL);
}

void CDlgNewCommentIE::OnClickClose()
{

}

void CDlgNewCommentIE::ShowDlgIESpecialSize(const CString &StrTitle,const CString &StrUrl,int bFlag/* = 0*/)
{
    if( 1 == bFlag )
    {
        if ( NULL == m_hWnd )
        {
            return;
        }
        ::SendMessage(m_hWnd, WM_SYSCOMMAND, SC_CLOSE, 0);
        return;
    }

    CRect rcShow;
    SystemParametersInfo(SPI_GETWORKAREA, 0, &rcShow, SPIF_SENDCHANGE);	// 不包括任务栏在内
    int screenWidth = rcShow.Width();
    rcShow.left = screenWidth * 3 / 16;
    rcShow.right = rcShow.left + screenWidth * 5 / 8;
    rcShow.top = 103;
    rcShow.bottom -= 60;

    if( m_hWnd == NULL )
    {
        Create(IDD, AfxGetMainWnd());
    }

    SetTitleUrl(StrTitle, StrUrl);
    MoveWindow(&rcShow);
    ShowWindow(SW_NORMAL);
    ShowWeb(rcShow);	// 显示浏览器内容
}

void CDlgNewCommentIE::ShowWeb(const CRect& rc)
{
    if(m_pWndCef != NULL)
    {
		m_pWndCef->OpenUrl(m_strUrl);
		return;
    }
    m_pWndCef = new CWndCef;
	m_pWndCef->CreateWndCef(7506,this, CRect(0, 0, rc.Width()-2*m_nFrameWidth, rc.Height()),m_strUrl);
  //  m_pWndCef->SetUrl(m_strUrl);
 //   m_pWndCef->Create(NULL,NULL, WS_CHILD|WS_VISIBLE,
  //      CRect(0, 0, rc.Width()-2*m_nFrameWidth, rc.Height()), this, 7506);
}

void CDlgNewCommentIE::OnPaint()
{
    CPaintDC dc(this); // device context for painting

}

BOOL CDlgNewCommentIE::OnEraseBkgnd(CDC* pDC) 
{
    return TRUE;
}

