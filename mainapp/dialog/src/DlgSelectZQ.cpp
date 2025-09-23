// DlgSelectZQ.cpp : implementation file
//

#include "stdafx.h"

#include "DlgSelectZQ.h"
#include "ShareFun.h"
#include "CFormularContent.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgSelectZQ dialog


CDlgSelectZQ::CDlgSelectZQ(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgSelectZQ::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgSelectZQ)
	m_bFS = FALSE;
	m_bM1 = FALSE;
	m_bM15 = FALSE;
	m_bM30 = FALSE;
	m_bM5 = FALSE;
	m_bM60 = FALSE;
	m_bM180 = FALSE;
	m_bM240 = FALSE;
	m_bDay	= FALSE;
	m_bMonth = FALSE;
	m_bQuarter = FALSE;
	m_bTrade = FALSE;
	m_bWeek = FALSE;
	m_bYear = FALSE;
	m_bDayUser = FALSE;
	m_bMUser = FALSE;
	flag = 0;
	//}}AFX_DATA_INIT
}


void CDlgSelectZQ::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgSelectZQ)
	DDX_Check(pDX, IDC_CHECK_FS, m_bFS);
	DDX_Check(pDX, IDC_CHECK_MINIUTE1, m_bM1);
	DDX_Check(pDX, IDC_CHECK_MINIUTE15, m_bM15);
	DDX_Check(pDX, IDC_CHECK_MINIUTE30, m_bM30);
	DDX_Check(pDX, IDC_CHECK_MINIUTE5, m_bM5);
	DDX_Check(pDX, IDC_CHECK_MINIUTE60, m_bM60);
	DDX_Check(pDX, IDC_CHECK_MINIUTE180, m_bM180);
	DDX_Check(pDX, IDC_CHECK_MINIUTE240, m_bM240);
	DDX_Check(pDX, IDC_CHECK_MONTH, m_bMonth);
	DDX_Check(pDX, IDC_CHECK_DAY, m_bDay);
	DDX_Check(pDX, IDC_CHECK_QUARTER, m_bQuarter);
	DDX_Check(pDX, IDC_CHECK_TRADE, m_bTrade);
	DDX_Check(pDX, IDC_CHECK_WEEK, m_bWeek);
	DDX_Check(pDX, IDC_CHECK_YEAR, m_bYear);
	DDX_Check(pDX, IDC_CHECK_MINIUTE_USER, m_bMUser);
	DDX_Check(pDX, IDC_CHECK_DAY_USER, m_bDayUser);

	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDlgSelectZQ, CDialogEx)
	//{{AFX_MSG_MAP(CDlgSelectZQ)
	ON_COMMAND( IDC_BUTTON_ADD_ALL, OnAddAll)
	ON_COMMAND( IDC_BUTTON_DEL_ALL, OnDelAll)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgSelectZQ message handlers
BOOL CDlgSelectZQ::OnInitDialog() 
{
	CDialog::OnInitDialog();
	HICON hIcon = LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME));
	SetIcon(hIcon,FALSE);	
	
	if ( CheckFlag ( flag, CFormularContent::KAllowMinute))
	{
		m_bM1 = TRUE;
	}
	if ( CheckFlag ( flag, CFormularContent::KAllowMinute5))
	{
		m_bM5 = TRUE;
	}
	if ( CheckFlag ( flag, CFormularContent::KAllowMinute15))
	{
		m_bM15 = TRUE;
	}
	if ( CheckFlag ( flag, CFormularContent::KAllowMinute30))
	{
		m_bM30 = TRUE;
	}
	if ( CheckFlag ( flag, CFormularContent::KAllowMinute60))
	{
		m_bM60 = TRUE;
	}
	if ( CheckFlag ( flag, CFormularContent::KAllowMinute180))
	{
		m_bM180 = TRUE;
	}
	if ( CheckFlag ( flag, CFormularContent::KAllowMinute240))
	{
		m_bM240 = TRUE;
	}
	if ( CheckFlag ( flag, CFormularContent::KAllowDay))
	{
		m_bDay = TRUE;
	}
	if ( CheckFlag ( flag, CFormularContent::KAllowWeek))
	{
		m_bWeek = TRUE;
	}
	if ( CheckFlag ( flag, CFormularContent::KAllowMonth))
	{
		m_bMonth = TRUE;
	}
	if ( CheckFlag ( flag, CFormularContent::KAllowQuarter))
	{
		m_bQuarter = TRUE;
	}
	if ( CheckFlag ( flag, CFormularContent::KAllowYear))
	{
		m_bYear = TRUE;
	}
	if ( CheckFlag ( flag, CFormularContent::KAllowTrend))
	{
		m_bFS = TRUE;
	}
	if ( CheckFlag ( flag, CFormularContent::KAllowTick))
	{
		m_bTrade = TRUE;
	}
	if ( CheckFlag ( flag, CFormularContent::KAllowDayUser))
	{
		m_bDayUser = TRUE;
	}
	if ( CheckFlag ( flag, CFormularContent::KAllowMinuteUser))
	{
		m_bMUser = TRUE;
	}

	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgSelectZQ::OnOK() 
{
	UpdateData();

	int32 inc = 0;
	flag = 0;
	if ( m_bM1 )
	{
		AddFlag(flag,CFormularContent::KAllowMinute);
		inc++;
	}
	if ( m_bM5 )
	{
		AddFlag(flag,CFormularContent::KAllowMinute5);
		inc++;
	}
	if ( m_bM15 )
	{
		AddFlag(flag,CFormularContent::KAllowMinute15);
		inc++;
	}
	if ( m_bM30 )
	{
		AddFlag(flag,CFormularContent::KAllowMinute30);
		inc++;
	}
	if ( m_bM60 )
	{
		AddFlag(flag,CFormularContent::KAllowMinute60);
		inc++;
	}
	if ( m_bM180 )
	{
		AddFlag(flag,CFormularContent::KAllowMinute180);
		inc++;
	}
	if ( m_bM240 )
	{
		AddFlag(flag,CFormularContent::KAllowMinute240);
		inc++;
	}
	if ( m_bDay )
	{
		AddFlag(flag,CFormularContent::KAllowDay);
		inc++;
	}
	if ( m_bWeek )
	{
		AddFlag(flag,CFormularContent::KAllowWeek);
		inc++;
	}
	if ( m_bMonth )
	{
		AddFlag(flag,CFormularContent::KAllowMonth);
		inc++;
	}
	if ( m_bQuarter )
	{
		AddFlag(flag,CFormularContent::KAllowQuarter);
		inc++;
	}
	if ( m_bYear )
	{
		AddFlag(flag,CFormularContent::KAllowYear);
		inc++;
	}
	if ( m_bFS )
	{
		AddFlag(flag,CFormularContent::KAllowTrend);
		inc++;
	}
	if ( m_bTrade )
	{
		AddFlag(flag,CFormularContent::KAllowTick);
		inc++;
	}
	if ( m_bDayUser)
	{
		AddFlag(flag,CFormularContent::KAllowDayUser);
		inc++;
	}
	if ( m_bMUser)
	{
		AddFlag(flag,CFormularContent::KAllowMinuteUser);
		inc++;
	}
	if ( 0 == inc )
	{
		MessageBox(_T("您必须至少选择一个周期."), AfxGetApp()->m_pszAppName, MB_ICONWARNING);
		return;
	}

	CDialog::OnOK();
}

void CDlgSelectZQ::OnAddAll()
{
	m_bM1		= true;
	m_bM15		= true;
	m_bM30		= true;
	m_bM5		= true;
	m_bM60		= true;
	m_bM180		= true;
	m_bM240		= true;
	m_bDay		= true;
	m_bMonth	= true;
	m_bQuarter	= true;
	m_bWeek		= true;
	m_bYear		= true;
	m_bMUser	= true;
	m_bDayUser	= true;
	
	UpdateData(false);
}

void CDlgSelectZQ::OnDelAll()
{
	m_bM1		= false;
	m_bM15		= false;
	m_bM30		= false;
	m_bM5		= false;
	m_bM60		= false;
	m_bM180		= false;
	m_bM240		= false;
	m_bDay		= false;
	m_bMonth	= false;
	m_bQuarter	= false;
	m_bWeek		= false;
	m_bYear		= false;
	m_bMUser	= false;
	m_bDayUser	= false;

	UpdateData(false);
}