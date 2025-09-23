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
	m_StrResponsibility =   L"                            ������������";
	m_StrResponsibility +=	L"\r\n\r\n1������˾�������ṩ�̶ԲƸ�������ݵ���ʵ�ԡ�׼ȷ�ԡ������Բ����κ���ʽ�ĵ�������Ϊ������ͨѶ����ĸ������أ����Բ���������ɿ��ԣ��Է���ļ�ʱ�ԡ���ȫ�ԡ�������������������";
	m_StrResponsibility +=	L"\r\n\r\n2������˾�������ṩ�̶��û����ݲƸ���ܽ��н���Ͷ������ɵ�ӯ�����е��κ����κ����񡣴���������������������������ݺ���Ϣ�����ο����û����ݲƸ�����������κν��׾���Ӧ���и���";
	
	m_StrResponsibility +=	L"\r\n\r\n3������˾�������ṩ�̶����κ���ʹ�û��޷�ʹ�òƸ���ܶ�ֱ�ӻ���������⳥����ʧ��ծ������κν�����ֹ�����е����κ�����";
	m_StrResponsibility +=	L"\r\n\r\n4������˾��Ȩ�û���Э��Լ���ķ��������ڿ���ʹ�òƸ�����������ֵ���񡣲Ƹ���������Ϊ�ͻ�����������������ϵͳ��һ����ɲ��֣��κ�����£�����˾�����κοͻ�ת�ñ����������Ȩ��";
	
	m_StrResponsibility += L"\r\n\r\n  ������������Ч��";

	{
		CGGTongApp* pApp = (CGGTongApp*)AfxGetApp();
		m_StrResponsibility.Replace(_T("�Ƹ����"), pApp->m_StrAppNameConfig);
		// m_StrResponsibility.Replace(_T("�Ƹ����"), CVersionInfo::Instance()->GetVersionName());
	}
	*/
	CRect rcClient;
	GetClientRect(&rcClient);
	rcClient.right += 19; // ���ع�����

	rcClient.left   += 3;
	rcClient.bottom -= 30;

	//
	m_RichEdit.MoveWindow(rcClient);

	CString StrName = CPathFactory::GetPublicConfigPath();
	StrName += L"notifyclause.rtf";
	m_RichEdit.ReadRtfFile( StrName );

	m_RichEdit.SetOneFormat(gFontFactory.GetExistFontName(L"΢���ź�"), RGB(230, 230, 250));	//...

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