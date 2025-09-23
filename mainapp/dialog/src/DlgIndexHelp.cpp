// DlgWelcome.cpp : implementation file
//
#include "stdafx.h"
#include "DlgIndexHelp.h"

#include "pathfactory.h"
#include "GdiPlusTS.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgIndexHelp dialog

CDlgIndexHelp::CDlgIndexHelp(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgIndexHelp::IDD, pParent)
{
	m_strShowImg = L"";
	m_pImgHelp = NULL;
	m_bHelpDlg = TRUE;
	m_hIcon = NULL;
}

CDlgIndexHelp:: ~CDlgIndexHelp()
{
	
}

void CDlgIndexHelp::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgIndexHelp)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDlgIndexHelp, CDialogEx)
	//{{AFX_MSG_MAP(CDlgIndexHelp)
	ON_WM_SYSCOMMAND()
	ON_WM_QUERYDRAGICON()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgIndexHelp message handlers

BOOL CDlgIndexHelp::OnInitDialog()
{
	int32 iWidth = 740, iHeight = 525;

	m_pImgHelp = Image::FromFile(m_strShowImg);
	if (NULL != m_pImgHelp)
	{
		iWidth = m_pImgHelp->GetWidth() + 6;
		iHeight = m_pImgHelp->GetHeight() + 33;
	}
	else
	{
		return FALSE;
	}

	MoveWindow(0, 0, iWidth, iHeight);
	CenterWindow();
	
	return TRUE; 
}

void CDlgIndexHelp::PostNcDestroy()
{
	CDialog::PostNcDestroy();
	DEL(m_pImgHelp);
	delete this;
}

void CDlgIndexHelp::OnPaint()
{
	CPaintDC dc(this);

	CRect rc;
	GetWindowRect(rc);
	ScreenToClient(&rc);

	rc.top = 0;
	rc.left = 0;
	rc.right = rc.Width() - 3;
	rc.bottom = rc.Height() - 3;

	if (NULL != m_pImgHelp )
	{
		Graphics g(dc.m_hDC);
		DrawImage(g, m_pImgHelp, rc, 1, 0, true);
	}
}

void CDlgIndexHelp::OnSysCommand(UINT nID, LPARAM lParam)
{
	CDialog::OnSysCommand(nID, lParam);
}

HCURSOR CDlgIndexHelp::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CDlgIndexHelp::SetShowImg(CString strImg)
{
	m_strShowImg = strImg;
}