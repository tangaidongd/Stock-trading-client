// dlgresponsibility.cpp : implementation file
//

#include "stdafx.h"

#include "dlgresponsibility.h"
#include "PathFactory.h"
#include "FontFactory.h"
#include "ConfigInfo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgResponsibility dialog


CDlgResponsibility::CDlgResponsibility(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgResponsibility::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgResponsibility)
	//m_StrResponsibility = _T("");
	//}}AFX_DATA_INIT
}


void CDlgResponsibility::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgResponsibility)
	DDX_Control(pDX, IDC_RICHEDIT1, m_RichEdit);//m_StrResponsibility);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDlgResponsibility, CDialogEx)
	//{{AFX_MSG_MAP(CDlgResponsibility)
		// NOTE: the ClassWizard will add message map macros here
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgResponsibility message handlers

BOOL CDlgResponsibility::OnInitDialog()
{
	CDialog::OnInitDialog();
	/*
	m_StrResponsibility =   L"                            免责声明条款";
	m_StrResponsibility +=	L"\r\n\r\n1、本公司和内容提供商对财富金盾内容的真实性、准确性、完整性不做任何形式的担保。因为互联网通讯方面的各种因素，所以不担保服务可靠性，对服务的及时性、安全性、出错发生都不作担保。";
	m_StrResponsibility +=	L"\r\n\r\n2、本公司和内容提供商对用户依据财富金盾进行金融投资所造成的盈亏不承担任何责任和义务。此项服务及其内容所包含的所有数据和信息仅供参考，用户依据财富金盾所做的任何交易决定应自行负责。";
	
	m_StrResponsibility +=	L"\r\n\r\n3、本公司和内容提供商对于任何因使用或无法使用财富金盾而直接或间接引起的赔偿、损失、债务或是任何交易中止均不承担责任和义务。";
	m_StrResponsibility +=	L"\r\n\r\n4、本公司授权用户在协议约定的服务期限内可以使用财富金盾软件及增值服务。财富金盾软件作为客户端软件，是整体软件系统的一个组成部分；任何情况下，本公司不向任何客户转让本软件的所有权。";
	
	m_StrResponsibility += L"\r\n\r\n  此声明永久有效。";

	{
		CGGTongApp* pApp = (CGGTongApp*)AfxGetApp();
		m_StrResponsibility.Replace(_T("财富金盾"), pApp->m_StrAppNameConfig);
		// m_StrResponsibility.Replace(_T("财富金盾"), CVersionInfo::Instance()->GetVersionName());
	}
	*/
	CRect rcClient;
	GetClientRect(&rcClient);
	rcClient.right += 19; // 隐藏滚动条

	rcClient.left   += 3;
	rcClient.bottom -= 30;

	//
	m_RichEdit.MoveWindow(rcClient);

	CString StrName = CPathFactory::GetPublicConfigPath();
	StrName += L"notifyclause.rtf";
	m_RichEdit.ReadRtfFile( StrName );

	m_RichEdit.SetOneFormat(gFontFactory.GetExistFontName(L"微软雅黑"), RGB(230, 230, 250));	//...

	UpdateData(false);
	return TRUE;
}

void CDlgResponsibility::OnPaint() 
{
	CPaintDC dc(this);
	CRect rcClient;
	GetClientRect(&rcClient);
	dc.FillSolidRect(rcClient, 0x4d4137);
}