#include "StdAfx.h"
#include "DlgQRCode.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
//////////////////////////////////////////////////////////////////////////
Image	*CDlgQRCode::m_pImage = NULL;
//////////////////////////////////////////////////////////////////////////
CDlgQRCode *CDlgQRCode::m_spThis = NULL;

IMPLEMENT_DYNCREATE(CDlgQRCode, CDialogEx)


CDlgQRCode::CDlgQRCode( CWnd *pParent /*= NULL*/ )
:CDialogEx(IDD, pParent)
{

}

void CDlgQRCode::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgQRCode)
	//}}AFX_DATA_MAP
}

BOOL CDlgQRCode::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	SetWindowText(_T("国海证券开户享万2.5佣金"));
	
	m_pImage = Image::FromFile(L".//image//brokerReg.png");
	
	RecalcLayout();
	//
	return FALSE;
}

BEGIN_MESSAGE_MAP(CDlgQRCode, CDialogEx)
//{{AFX_MSG_MAP(CDlgQRCode)
ON_WM_GETMINMAXINFO()
ON_WM_SIZE()
ON_WM_DESTROY()
ON_WM_ERASEBKGND()
ON_WM_CONTEXTMENU()
ON_WM_PAINT()
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CDlgQRCode::OnSize( UINT nType, int cx, int cy )
{
	CDialogEx::OnSize(nType, cx, cy);

	RecalcLayout();
	
	Invalidate(TRUE);		// 
}

void CDlgQRCode::RecalcLayout()
{
		
}

void CDlgQRCode::PostNcDestroy()
{
	if ( m_spThis == this )
	{
		m_spThis = NULL;
	}

	DEL(m_pImage);

	delete this;
}

void CDlgQRCode::OnDestroy()
{
	CDialogEx::OnDestroy();
}

void CDlgQRCode::OnPaint()
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
	fRect.X = (REAL)0;
	fRect.Y = (REAL)0;
	fRect.Width = (REAL)iWidth;
	fRect.Height = (REAL)iHeight;

	graphics.DrawImage(m_pImage, fRect, 0, 0, (REAL)iWidth, (REAL)iHeight, UnitPixel);

	dc.BitBlt(0, 0, rcWindow.Width(), rcWindow.Height(), &memDC, 0, 0, SRCCOPY);
	dc.SelectClipRgn(NULL);
	memDC.DeleteDC();
	bmp.DeleteObject();
}

BOOL CDlgQRCode::OnEraseBkgnd( CDC* pDC )
{
	return TRUE;	
}

void CDlgQRCode::OnCancel()
{
	// 销毁还是隐藏呢？
	DestroyWindow();
}

void CDlgQRCode::OnContextMenu( CWnd* pWnd, CPoint pos )
{
	//
}

void CDlgQRCode::OnGetMinMaxInfo( MINMAXINFO* lpMMI )
{
	if ( NULL != lpMMI )
	{
		lpMMI->ptMinTrackSize.x = 640;
		lpMMI->ptMinTrackSize.y = 480;
	}
}

void CDlgQRCode::ShowPopupQRCode()
{
    if ( NULL == m_spThis )
    {
        m_spThis = new CDlgQRCode(AfxGetMainWnd());
        m_spThis->Create(IDD, AfxGetMainWnd());
    }

	CRect rect(275, 100, 1075, 700);
	
	if (NULL != m_pImage && Ok == m_pImage->GetLastStatus())
	{
		rect.SetRect(0, 0, m_pImage->GetWidth() + 6, m_pImage->GetHeight() + 30);
	}

	m_spThis->MoveWindow(&rect);
	m_spThis->CenterWindow();

	m_spThis->ShowWindow(SW_SHOW);
	m_spThis->UpdateWindow();

}

BOOL CDlgQRCode::PreTranslateMessage( MSG* pMsg )
{
	return CDialogEx::PreTranslateMessage(pMsg);
}
