// dlgconnectset.cpp : implementation file
//
#include "stdafx.h"

#include "PathFactory.h"
#include "dlgconnectset.h"
#include "dlghostset.h"
#include "ShareFun.h"
#include "coding.h"
#include "CCodeFile.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgConnectSet dialog

CDlgConnectSet::CDlgConnectSet(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgConnectSet::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgConnectSet)
	m_bCheckProxy = FALSE;
	m_bCheckGreen = FALSE;
	m_bCheckManual = FALSE;
	m_iRadio = -1;
	m_uiProxyPort = 0;
	m_StrProxyAdress = _T("");
	m_StrProxyName = _T("");
	m_StrProxyPwd = _T("");
	//}}AFX_DATA_INIT
	m_iProxyType = 0;
	m_pSocketClient = new CSocketClient(*this);
	m_pSocketClient->Construct();
	m_StrUserName = L"";
	m_iHostIndex  = 0;
}

CDlgConnectSet::~CDlgConnectSet()
{
	delete m_pSocketClient;
	m_pSocketClient = NULL;
}
void CDlgConnectSet::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgConnectSet)
	DDX_Control(pDX, IDC_CHECK_PROXY, m_CtrlCheckProxy);
	DDX_Control(pDX, IDC_EDIT_PROXY_PWD, m_CtrlEditProxyPwd);
	DDX_Control(pDX, IDC_EDIT_PROXY_NAME, m_CtrlEditProxyName);
	DDX_Control(pDX, IDC_EDIT_PORT, m_CtrlEditProxyPort);
	DDX_Control(pDX, IDC_EDIT_ADDRESS, m_CtrlEditProxyAdress);
	DDX_Control(pDX, IDC_COMBO_SERVER, m_ComboHost);
	DDX_Check(pDX, IDC_CHECK_PROXY, m_bCheckProxy);
	DDX_Check(pDX, IDC_CHECK_MANUAL, m_bCheckManual);			
	//DDX_Radio(pDX, IDC_RADIO_HTTP, m_iRadio);
	DDX_Text(pDX, IDC_EDIT_PORT, m_uiProxyPort);
	DDX_Text(pDX, IDC_EDIT_ADDRESS, m_StrProxyAdress);
	DDX_Text(pDX, IDC_EDIT_PROXY_NAME, m_StrProxyName);
	DDX_Text(pDX, IDC_EDIT_PROXY_PWD, m_StrProxyPwd);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_CHECK_GREEN_CHANNEL, m_CtrlCheckGreen);
	DDX_Check(pDX, IDC_CHECK_GREEN_CHANNEL, m_bCheckGreen);
}
BEGIN_MESSAGE_MAP(CDlgConnectSet, CDialogEx)
	//{{AFX_MSG_MAP(CDlgConnectSet)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_CHECK_PROXY, OnCheckProxy)
	ON_BN_CLICKED(IDC_BUTTON_TEST, OnButtonTest)
	ON_BN_CLICKED(IDC_RADIO_SOCK4, OnRadioSock4)
	ON_BN_CLICKED(IDC_RADIO_SOCK5, OnRadioSock5)
	ON_BN_CLICKED(IDC_RADIO_HTTP, OnRadioHttp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgConnectSet message handlers
BOOL CDlgConnectSet::OnInitDialog() 
{
	CDialog::OnInitDialog();

	HICON hIcon = LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME));
	SetIcon(hIcon,FALSE);
	m_CtrlCheckGreen.SetWindowText(_T("启用绿色通道（如您的网络屏蔽80端口，请勾选后再行登录）"));
	
	// 代理
	switch(m_ProxyInfo.uiProxyType)
	{
	case 0:
		// ... fangz0513# 直连
		break;
	case 1:				// HTTP
		{
			m_StrProxyAdress = m_ProxyInfo.StrProxyAddress;
			m_uiProxyPort	 = m_ProxyInfo.uiProxyPort;
			m_StrProxyName   = m_ProxyInfo.StrUserName;
			m_StrProxyPwd	 = m_ProxyInfo.StrUserPwd;  
			OnRadioHttp();
		}
		break;
	case 2:				//SOCK4
		{
			m_StrProxyAdress = m_ProxyInfo.StrProxyAddress;
			m_uiProxyPort	 = m_ProxyInfo.uiProxyPort;
			m_StrProxyName   = L"";
			m_StrProxyPwd	 = L"";
			OnRadioSock4();
		}
	    break;
	case 3:				//SOCK5
		{
			m_StrProxyAdress = m_ProxyInfo.StrProxyAddress;
			m_uiProxyPort	 = m_ProxyInfo.uiProxyPort;
			m_StrProxyName   = m_ProxyInfo.StrUserName;
			m_StrProxyPwd	 = m_ProxyInfo.StrUserPwd;
			OnRadioSock5();
		}
	    break;
	default:
	    break;
	}
	
	GetServerInfoFromXml();

	//
	int32 iSize = m_aHostInfo.GetSize();
	for (int32 i = 0; i < iSize; i++)
	{
		m_ComboHost.InsertString(i, m_aHostInfo.GetAt(i).StrHostName);
	}

	if (iSize > 0 && m_iHostIndex >= 0 && m_iHostIndex < m_ComboHost.GetCount())
	{
		m_ComboHost.SetCurSel(m_iHostIndex);
	}
	else
	{
		m_ComboHost.SetCurSel(0);
	}


	//
	SetProxyCtrlStates(m_bCheckProxy);
	UpdateData(false);

	return TRUE;  
	              
}

void CDlgConnectSet::SetInitialProxyInfo( const T_ProxyInfo& ProxyInfo )
{
	m_ProxyInfo		= ProxyInfo;
}

void CDlgConnectSet::SetInitialInfo(const CString& StrUserName, bool32 bUserProxy, int32 iHostIndex, bool32 bManual, bool32 bGreenChannel)
{
	m_bCheckProxy	= bUserProxy;
	m_StrUserName	= StrUserName;
	m_iHostIndex	= iHostIndex;
	m_bCheckManual  = bManual;
	m_bCheckGreen   = bGreenChannel;
}

bool32 CDlgConnectSet::SetProxyCtrlStates(bool32 bCheck)
{
	if (bCheck)
	{
		m_CtrlEditProxyAdress.EnableWindow(true);
		m_CtrlEditProxyPort.EnableWindow(true);
		m_CtrlEditProxyName.EnableWindow(true);
		m_CtrlEditProxyPwd.EnableWindow(true);
		GetDlgItem(IDC_BUTTON_TEST)->EnableWindow(true);
		GetDlgItem(IDC_RADIO_SOCK4)->EnableWindow(true);
		GetDlgItem(IDC_RADIO_SOCK5)->EnableWindow(true);
		GetDlgItem(IDC_RADIO_HTTP)->EnableWindow(true);

		if ( ((CButton *)GetDlgItem(IDC_RADIO_SOCK4))->GetCheck() )
		{
			m_CtrlEditProxyName.EnableWindow(false);
			m_CtrlEditProxyPwd.EnableWindow(false);
		}
	}
	else if (!bCheck)
	{
		m_CtrlEditProxyAdress.EnableWindow(false);
		m_CtrlEditProxyPort.EnableWindow(false);
		m_CtrlEditProxyName.EnableWindow(false);
		m_CtrlEditProxyPwd.EnableWindow(false);
		GetDlgItem(IDC_BUTTON_TEST)->EnableWindow(false);
		GetDlgItem(IDC_RADIO_SOCK4)->EnableWindow(false);
		GetDlgItem(IDC_RADIO_SOCK5)->EnableWindow(false);
		GetDlgItem(IDC_RADIO_HTTP)->EnableWindow(false);
	}
	
	return bCheck;
}

void CDlgConnectSet::GetHostString(IN CString StrHostInfo,OUT CString& StrHostName,OUT CString& StrHostAddress, OUT CString& StrHostPort)
{	
	int iStart,iMiddle,iEnd;
	iStart = StrHostInfo.Find('<');
	iMiddle = StrHostInfo.Find(':');
	iEnd = StrHostInfo.Find('>');

	 StrHostName    = StrHostInfo.Left(iStart); 
	 StrHostAddress = StrHostInfo.Mid(iStart+1,(iMiddle-iStart-1));
	 StrHostPort	= StrHostInfo.Mid(iMiddle+1,(iEnd-iMiddle-1));
}

void CDlgConnectSet::OnCheckProxy() 
{
	m_bCheckProxy = !m_bCheckProxy;
	SetProxyCtrlStates(m_bCheckProxy);
}
void CDlgConnectSet::OnButtonTest() 
{	
	if (m_pSocketClient)
	{
		delete m_pSocketClient;
		m_pSocketClient = NULL;		
	}
			
	m_pSocketClient = new CSocketClient(*this);
	m_pSocketClient->Construct();

	//	
	CString  StrServerAdd = L"www.baidu.com";		// 强制修改为baidu了？
	int32	 iServerPort  = 80;

	// 代理
	if (m_bCheckProxy)
	{
		UpdateData(true);

		CProxyInfo ProxyInfo;

		// 使用代理
		switch(m_iRadio)
		{
		case 0:										// HTTP 
			{
				ProxyInfo.m_eProxyType = CProxyInfo::EPTHttp;
				ProxyInfo.m_StrProxyAddr = m_StrProxyAdress;
				ProxyInfo.m_iProxyPort   = m_uiProxyPort;
				ProxyInfo.m_StrProxyUser = m_StrProxyName;
				ProxyInfo.m_StrProxyPassword = m_StrProxyPwd;
				m_pSocketClient->SetProxy(ProxyInfo);
				m_pSocketClient->ConnectServer(StrServerAdd,iServerPort);

			}
			break;
		case 1:										// SOCK4
			{				
				ProxyInfo.m_eProxyType = CProxyInfo::EPTSock4;
				ProxyInfo.m_StrProxyAddr = m_StrProxyAdress;
				ProxyInfo.m_iProxyPort   = m_uiProxyPort;
				ProxyInfo.m_StrProxyUser = m_StrProxyName;
				ProxyInfo.m_StrProxyPassword = m_StrProxyPwd;				
				m_pSocketClient->SetProxy(ProxyInfo);
				m_pSocketClient->ConnectServer(StrServerAdd,iServerPort);												
			}
			break;
		case 2:										// SOCK5
			{
				ProxyInfo.m_eProxyType = CProxyInfo::EPTSock5;
				ProxyInfo.m_StrProxyAddr = m_StrProxyAdress;
				ProxyInfo.m_iProxyPort   = m_uiProxyPort;
				ProxyInfo.m_StrProxyUser = m_StrProxyName;
				ProxyInfo.m_StrProxyPassword = m_StrProxyPwd;			
				m_pSocketClient->SetProxy(ProxyInfo);
				m_pSocketClient->ConnectServer(StrServerAdd,iServerPort);				
			}
		    break;
		default:
		    break;
		}		
		
	}
}
void CDlgConnectSet::OnRadioHttp() 
{
	CButton* RadioHttp =(CButton *)GetDlgItem(IDC_RADIO_HTTP);
	CButton* RadioSock4 =(CButton *)GetDlgItem(IDC_RADIO_SOCK4);
	CButton* RadioSock5 =(CButton *)GetDlgItem(IDC_RADIO_SOCK5);
	RadioHttp->SetCheck(1);
	RadioSock4->SetCheck(0);
	RadioSock5->SetCheck(0);	
	m_CtrlEditProxyName.EnableWindow(true);
	m_CtrlEditProxyPwd.EnableWindow(true);
	if ( 0 == m_uiProxyPort )
	{
		m_uiProxyPort = 80;
	}

	m_iRadio = 0;
	m_iProxyType = 1;
	UpdateData(false);

}
void CDlgConnectSet::OnRadioSock4() 
{
	
	CButton* RadioHttp =(CButton *)GetDlgItem(IDC_RADIO_HTTP);
	CButton* RadioSock4 =(CButton *)GetDlgItem(IDC_RADIO_SOCK4);
	CButton* RadioSock5 =(CButton *)GetDlgItem(IDC_RADIO_SOCK5);
	RadioHttp->SetCheck(0);
	RadioSock4->SetCheck(1);
	RadioSock5->SetCheck(0);
	
	m_StrProxyName = L"";
	m_StrProxyPwd = L""; 

	m_CtrlEditProxyName.EnableWindow(false);
	m_CtrlEditProxyPwd.EnableWindow(false);
	if ( 0 == m_uiProxyPort )
	{
		m_uiProxyPort = 1080;
	}

	m_iRadio = 1;
	m_iProxyType = 2;
	UpdateData(false);

}

void CDlgConnectSet::OnRadioSock5() 
{
	CButton* RadioHttp =(CButton *)GetDlgItem(IDC_RADIO_HTTP);
	CButton* RadioSock4 =(CButton *)GetDlgItem(IDC_RADIO_SOCK4);
	CButton* RadioSock5 =(CButton *)GetDlgItem(IDC_RADIO_SOCK5);
	RadioHttp->SetCheck(0);
	RadioSock4->SetCheck(0);
	RadioSock5->SetCheck(1);


	m_CtrlEditProxyName.EnableWindow(true);
	m_CtrlEditProxyPwd.EnableWindow(true);
	if ( 0 == m_uiProxyPort )
	{
		m_uiProxyPort = 1080;
	}

	m_iRadio = 2;
	m_iProxyType = 3;
	UpdateData(false);
}

void CDlgConnectSet::OnSocketError()
{
	MessageBox(L"SocketError!", AfxGetApp()->m_pszAppName, MB_ICONWARNING);
}

void CDlgConnectSet::OnSocketConnected()
{
	MessageBox(L"Connected!", AfxGetApp()->m_pszAppName, MB_ICONWARNING);
}

void CDlgConnectSet::OnSocketRecvOneOrMore()
{
	;
}

bool32 CDlgConnectSet::SetProxyInfoToXml()
{
	UpdateData(TRUE);

	CString StrProxyPath = CPathFactory::GetProxyInfoPath(m_StrUserName);	
	
	//
	TCHAR TStrPath[1024];
	lstrcpy(TStrPath, (CString)StrProxyPath);
	
	if (!_tcheck_if_mkdir(TStrPath))
	{
		return false;
	}	
	//
	
	CString StrXml;
	StrXml  = L"<?xml version=\"1.0\" encoding=\"utf-8\" standalone=\"yes\"?>\n";
	StrXml += L"<XMLDATA version=\"1.0\" app= \"ggtong\" data=\"ProxyInfo\">\n";
	StrXml += L"<";
	StrXml += KStrElementNameProxyInfo;
	StrXml += L">";
	
	CString StrProxyInfo;
	CString StrNameProxy = (CString)KStrElementNameProxy;
	CString StrAttriBeUseProxy = (CString)KStrElementAttriBeUseProxy;
	CString StrAttriProxyType = (CString)KStrElementAttriProxyType;
	CString StrAttriProxyAddr = (CString)KStrElementAttriProxyAddress;
	CString StrAttriProxyPort = (CString)KStrElementAttriProxyPort;
	CString StrAttriProxyUserName = (CString)KStrElementAttriProxyUserName;
	CString StrAttriProxyUserPwd = (CString)KStrElementAttriProxyUserPwd;
	CString StrAttriGreenChannel = (CString)KStrElementAttriGreenChannel;

	StrProxyInfo.Format( L"<%s  %s=\"%d\" %s=\"%d\" %s=\"%s\" %s=\"%d\" %s=\"%s\" %s=\"%s\" %s=\"%d\"/>\n",
						StrNameProxy.GetBuffer(),
						StrAttriBeUseProxy.GetBuffer(),		m_bCheckProxy,
						StrAttriProxyType.GetBuffer(),		m_iProxyType,
						StrAttriProxyAddr.GetBuffer(),		m_StrProxyAdress.GetBuffer(),
						StrAttriProxyPort.GetBuffer(),		m_uiProxyPort,
						StrAttriProxyUserName.GetBuffer(),	m_StrProxyName.GetBuffer(),
						StrAttriProxyUserPwd.GetBuffer(),	m_StrProxyPwd.GetBuffer(),
						StrAttriGreenChannel.GetBuffer(),	m_bCheckGreen);

	StrXml += StrProxyInfo;
	StrXml += L"</";
	StrXml += KStrElementNameProxyInfo;
	StrXml += L">";
	StrXml += L"</XMLDATA>";
	
	string sProxyPath = _Unicode2MultiChar(StrProxyPath);
	const char* pStrNumTmp = sProxyPath.c_str();
	SaveXmlFile(pStrNumTmp, StrXml);

	return true;
}

bool32 CDlgConnectSet::SetUserInfoToXml()
{
	// 修改服务器索引
	UpdateData(TRUE);


	CString StrPath = CPathFactory::GetUserInfoPath();
	//
	string sPath = _Unicode2MultiChar(StrPath);
	const char* StrUserInfoPath = sPath.c_str();
	CCodeFile::DeCodeFile(StrUserInfoPath);
	
	TiXmlDocument myDocument = TiXmlDocument(StrUserInfoPath);
	
	if ( !myDocument.LoadFile() )
	{
		return	false;
	}

	// UserInfo
	TiXmlElement*	pRootElement = myDocument.RootElement();
	if (NULL == pRootElement)
	{
		return	false;
	}
	
	// User
	pRootElement	=	pRootElement->FirstChildElement(KStrElementNameUserInfo);
	if ( NULL == pRootElement->Value() || 0 != strcmp(pRootElement->Value(), KStrElementNameUserInfo) )
	{
		// 没有用户信息就算了
		return	false;
	}

	TiXmlElement* pUserElement = pRootElement->FirstChildElement(KStrElementNameUser);
	
	int32 iServerIndex = m_iHostIndex;
	if ( iServerIndex < 0 || iServerIndex >= m_aHostInfo.GetSize())
	{
		iServerIndex = 0;
	}

	//
	while( NULL != pUserElement )
	{
		//先查找以前有这个的用户信息没
		//
		const char* StrUserName = pUserElement->Attribute(KStrElementAttriUserName);
		
		// 名称
		TCHAR TStrUserName[1024];
		memset(TStrUserName, 0, sizeof(TStrUserName));
		MultiCharCoding2Unicode(EMCCUtf8, StrUserName, strlen(StrUserName), TStrUserName, sizeof(TStrUserName) / sizeof(TCHAR));
		
		if ( m_StrUserName.Compare(TStrUserName) == 0 )
		{
			//找到了，改变一下属性就可以了
			pUserElement->SetAttribute(KStrElementAttriServerIndex, iServerIndex);
		}

		//
		pUserElement	=	pUserElement->NextSiblingElement(KStrElementNameUser);
	}

	bool bSave = myDocument.SaveFile();
	ASSERT( bSave );

	CCodeFile::EnCodeFile(StrUserInfoPath);

	return true;
}

void CDlgConnectSet::OnOK() 
{
	UpdateData(TRUE);
	m_iHostIndex = m_ComboHost.GetCurSel();

	// 写入XML 文件
	SetProxyInfoToXml();
	SetUserInfoToXml();
	
	CDialog::OnOK();
}

bool32 CDlgConnectSet::ReadProxyInfoFromXml(const CString &StrUserNameOrg)
{
	// 从私人目录下读取代理信息:
	CString StrUserName(StrUserNameOrg);
	if ( StrUserName.GetLength() <= 0 )
	{
		CGGTongDoc *pDoc = AfxGetDocument();
		if ( pDoc != NULL )
		{
			StrUserName = pDoc->m_pAbsCenterManager->GetUserName();
		}
		if ( StrUserName.IsEmpty() )
		{
			return false;
		}
	}
	
	CString StrPath = CPathFactory::GetProxyInfoPath(StrUserName);
	string sPath = _Unicode2MultiChar(StrPath);
	const char* StrProxyPath = sPath.c_str();
	TiXmlDocument myDocument = TiXmlDocument(StrProxyPath);
	if ( !myDocument.LoadFile())
	{
		return false;
	}
	
	// XMLDATA
	TiXmlElement* pRootElement = myDocument.RootElement();
	if ( NULL == pRootElement )
	{
		return false;
	}
	
	// ProxyInfo
	pRootElement = pRootElement->FirstChildElement();
	if (NULL == pRootElement)
	{
		return false;
	}
	
	if( NULL == pRootElement->Value() || 0 != strcmp(pRootElement->Value(), KStrElementNameProxyInfo) )
	{
		return false;
	}
	
	// Proxy	
	TiXmlElement* pProxyElement = pRootElement->FirstChildElement();
	
	if ( NULL == pProxyElement->Value() || 0 != strcmp(pProxyElement->Value(), KStrElementNameProxy))
	{
		return false;
	}
	
	// bUseProxy
	const char* StrUseProxy = pProxyElement->Attribute(KStrElementAttriBeUseProxy);
	
	if ( NULL == StrUseProxy )
	{
		return false;			
	}
	
	// ProxyType
	const char* StrProxyType = pProxyElement->Attribute(KStrElementAttriProxyType);
	
	if ( NULL == StrProxyType )
	{
		return false;
	}
	
	// ProxyAddress
	const char* StrProxyAddress = pProxyElement->Attribute(KStrElementAttriProxyAddress);
	
	if ( NULL == StrProxyAddress )
	{
		return false;
	}
	
	// ProxyPort
	const char* StrProxyPort = pProxyElement->Attribute(KStrElementAttriProxyPort);
	
	if ( NULL == StrProxyPort )
	{
		return false;
	}
	
	// ProxyUserName
	const char* StrProxyUserName = pProxyElement->Attribute(KStrElementAttriProxyUserName);
	
	// ProxyUserPwd
	const char* StrProxyUserPwd  = pProxyElement->Attribute(KStrElementAttriProxyUserPwd);
	
	
	// 赋值:
	m_ProxyInfo.uiProxyType	  = atoi(StrProxyType);
	m_ProxyInfo.StrProxyAddress = StrProxyAddress;
	m_ProxyInfo.uiProxyPort	  = atoi(StrProxyPort);
	m_ProxyInfo.StrUserName	  = _A2W(StrProxyUserName);
	m_ProxyInfo.StrUserPwd	  = _A2W(StrProxyUserPwd);
	
	m_StrUserName = StrUserName;
	m_bCheckProxy			  = atoi(StrUseProxy);

	return true;
}

void CDlgConnectSet::GetServerInfoFromXml()
{
	// 从公共的config 目录下 Xml 文件中获得服务器信息:
	
	// 1: 清空本地数据:
	m_aHostInfo.RemoveAll();

	// 2: 读文件:
	CString StrPath  = CPathFactory::GetServerInfoPath();
	
	if (StrPath.IsEmpty() )
	{
		return;
	}
	
	string sFilePath = _Unicode2MultiChar(StrPath);
	const char* StrFilePath = sFilePath.c_str();
	TiXmlDocument myDocument = TiXmlDocument(StrFilePath);

	if ( !myDocument.LoadFile())
	{
		return;
	}

	// XMLDATA
	TiXmlElement* pRootElement = myDocument.RootElement();
	if ( NULL == pRootElement )
	{
		return;
	}
	
	// ServerInfoQuote, 行情服务器信息
	TiXmlElement* pQuoteServersElement = pRootElement->FirstChildElement(KStrElementNameServerInfoQuote);
	if( NULL != pQuoteServersElement && NULL != pQuoteServersElement->Value()  )
	{
		ASSERT( 0 == strcmp(pQuoteServersElement->Value(), KStrElementNameServerInfoQuote) );
		TiXmlElement* pServerElement = pQuoteServersElement->FirstChildElement();
		
		while(pServerElement)
		{	
			// 处理具体的Server节点:
			if ( NULL == pServerElement->Value() || 0 != strcmp(pServerElement->Value(), KStrElementNameServer))
			{
				pServerElement = pServerElement->NextSiblingElement();
				continue;
			}
			
			T_HostInfo ServerInfo;
			
			// 名称:
			const char* StrName = pServerElement->Attribute(KStrElementAttriServerName);
			
			if ( NULL == StrName )
			{
				pServerElement = pServerElement->NextSiblingElement();
				continue;
			}
			
			// IP:
			const char* StrIp = pServerElement->Attribute(KStrElementAttriServerIp);
			
			if ( NULL == StrIp )
			{
				pServerElement = pServerElement->NextSiblingElement();
				continue;
			}
			
			// 端口:
			const char* StrPort = pServerElement->Attribute(KStrElementAttriServerPort);
			
			if ( NULL == StrPort )
			{
				pServerElement = pServerElement->NextSiblingElement();
				continue;
			} 
			
			// 网络类型:
			UINT uiNetType = 1;

			const char* StrNetType = pServerElement->Attribute(KStrElementAttriServerNetType);
			if ( NULL != StrNetType )
			{
				uiNetType = atoi(StrNetType);				
			}

			// 转换编码:
			TCHAR TStrHostName[1024];
			memset(TStrHostName, 0, sizeof(TStrHostName));
			MultiCharCoding2Unicode(EMCCUtf8, StrName, strlen(StrName), TStrHostName, sizeof(TStrHostName) / sizeof(TCHAR));
			
			//
			TCHAR TStrHostAdd[1024];
			memset(TStrHostAdd, 0, sizeof(TStrHostAdd));
			MultiCharCoding2Unicode(EMCCUtf8, StrIp, strlen(StrIp), TStrHostAdd, sizeof(TStrHostAdd) / sizeof(TCHAR));
			
			//
			ServerInfo.StrHostName		= TStrHostName;
			ServerInfo.StrHostAddress	= TStrHostAdd;
			ServerInfo.uiHostPort		= (UINT)atoi(StrPort);
			ServerInfo.uiNetType		= uiNetType;

			m_aHostInfo.Add(ServerInfo);
			
			// 处理下一个:
			pServerElement = pServerElement->NextSiblingElement();
		}		
	}
}

HBRUSH CDlgConnectSet::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);
	return hbr;
}