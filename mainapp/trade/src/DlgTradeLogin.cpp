#include "StdAfx.h"
#include "DlgTradeLogin.h"
#include "proxy_auth_client_base.h"
#include "XmlShare.h"
#include "GdiPlusTS.h"
#include "ColorStep.h"
#include "PathFactory.h"
#include "ShareFun.h"
#include "DlgTodayComment.h"
#include "FontFactory.h"
#include "coding.h"
#include "DlgTradeOpen.h"
#include "DlgTradeClose.h"
#include "FontFactory.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const char*	  KStrElementAttriVesion = "1.0";

namespace
{
	static const TCHAR		KStrTradeLoginSection[]		= _T("TradeLogin");
	static const TCHAR		KStrTradeLoginKeySaveUser[]		= _T("SaveUser");
	static const TCHAR		KStrTradeLoginKeyUser[]			= _T("User");
	static const TCHAR		KStrTradeLoginKeyServerIP[]		= _T("ServerIP");
	static const TCHAR		KStrTradeLoginKeyServerPort[]	= _T("ServerPort");
	static const TCHAR		KStrTradeLoginKeySaveProtect[]	= _T("SaverProtect");
	static const short		KiBtnCloseID	= 100;

	#define WM_GETVALIDCODE_AFTERLOGIN		WM_USER+100
	#define INVALID_ID			-1
	#define ID_LOGIN_SETTING	10000
	#define ID_LOGIN_CLOSE		10001 
	#define ID_LOGIN_CONNECT	10002
	#define ID_LOGIN_CHECK_USER  10003
	#define ID_LOGIN_CHECK_PUSER 10004
}

BEGIN_MESSAGE_MAP(CDlgTradeLogin, BaseDialog)
//{{AFX_MSG_MAP(CDlgTradeLogin)
ON_WM_SIZE()
ON_WM_ERASEBKGND()
ON_WM_PAINT()
ON_STN_CLICKED(IDC_STATIC_IMAGE, OnChangeValidCode)
ON_WM_NCHITTEST()
ON_WM_MOUSEMOVE()
ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave) 
ON_WM_LBUTTONDOWN()
ON_WM_LBUTTONUP()
ON_WM_CTLCOLOR()
ON_EN_CHANGE(IDC_EDIT_TRADEUSER, OnEnChangeEditTradeUser)
ON_MESSAGE(WM_GETVALIDCODE_AFTERLOGIN,OnMsgGetValidcode)
ON_BN_CLICKED(IDC_BTN_REG, OnButtonApply)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CDlgTradeLogin::CDlgTradeLogin(E_TradeLoginType eTradeType, CWnd* pParent /*= NULL*/ )
: BaseDialog(IDD, pParent)
,m_StaticPrompt(*new CDlgStatic())
, m_ptLastCenter(-1,-1)
, m_bSaveUser(FALSE)
, m_bProtect(FALSE)
{
	m_stLoginInfo.eTradeLoginType = eTradeType;
	m_pLogin = NULL;
	m_eCurLoginStatus = ELSNotLogin;

	m_pDataValidCode = NULL;
	m_iLenImgValidCode = 0;
	m_pImageValidCode = NULL;
	m_iCloseBtnState = 0;
	m_pImageLogin = NULL;
	m_ptLastPress    = CPoint(-1,-1);
	memset(m_chRegisterAddress,0,sizeof(m_chRegisterAddress));
	m_iLinkID = 0;
	m_bUseProxy = false;
	m_sValidCode = L"";
	m_pImgCaption	= NULL;
	m_pImgBg		= NULL;
	m_pImgLogo		= NULL;
	m_pImgCheck		= NULL;
	m_pImgUnCheck   = NULL;
	m_pImgLoginClose    = NULL;
	m_pImgLoginSetting  = NULL;
	m_pImgLoginConnect  = NULL;


	m_iXButtonHovering = INVALID_ID;
}

CDlgTradeLogin::~CDlgTradeLogin()
{
//	DEL(m_pBtnClose);	
	DEL(m_pImageValidCode);
	DEL(m_pImageLogin);
	DEL(m_pImgCaption);
	DEL(m_pImgBg);
	DEL(m_pImgLogo);
	DEL(m_pImgCheck);
	DEL(m_pImgUnCheck);
	DEL(m_pImgLoginClose);
	DEL(m_pImgLoginSetting);
	DEL(m_pImgLoginConnect);

	m_fontStaticText.DeleteObject();
}

void CDlgTradeLogin::DoDataExchange( CDataExchange* pDX )
{
	BaseDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgTradeLogin)
	DDX_Control(pDX, IDC_COMBO_TRADESERVER, m_ComboServer);
	DDX_Control(pDX, IDC_EDIT_TRADEUSER, m_EditUser);
	DDX_Control(pDX, IDC_EDIT_TRADEPWD, m_EditPassword);
	DDX_Control(pDX, IDC_STATIC_PROMPT, m_StaticPrompt);
	DDX_Control(pDX, IDC_EDIT_VALIDCODE, m_CtrlEditValidCode);
	DDX_Text(pDX, IDC_EDIT_VALIDCODE, m_StrValidCode);
	DDX_Control(pDX, IDC_BTN_REG, m_CtrBtnReg);
	//}}AFX_DATA_MAP
}

BOOL CDlgTradeLogin::OnInitDialog()
{
	//CDialog::OnInitDialog();
	int32 iTitleHeight = 30;
	int32 iWndWidth = 396;
	int32 iWndHeight = 258;
	int32 iBgHeight = 228;
	CRect rcWnd(0, 0, iWndWidth, iWndHeight);
	
	m_rcCaption = rcWnd;
	m_rcCaption.bottom = iTitleHeight;
	
	m_rcBg = rcWnd;
	m_rcBg.top = m_rcCaption.bottom;
	m_rcBg.bottom = m_rcBg.top + iBgHeight;
	MoveWindow(rcWnd);

	m_pImgCaption = Image::FromFile(CPathFactory::GetImageLoginCaptionPath());
	m_pImgBg = Image::FromFile(CPathFactory::GetImageTradeBgImgPath());
	m_pImgLogo = Image::FromFile(CPathFactory::GetImageLoginLogoPath());
	m_pImgUnCheck = Image::FromFile(CPathFactory::GetImageLoginUnCheckPath());
	m_pImgCheck = Image::FromFile(CPathFactory::GetImageLoginCheckPath());

	m_pImgLoginClose = Image::FromFile(CPathFactory::GetImageLoginClosePath());
	int iHeight = (iTitleHeight - m_pImgLoginClose->GetHeight()/3)/2;
	CRect rcTmp(rcWnd.right-m_pImgLoginClose->GetWidth()-5, iHeight, rcWnd.right-5, iHeight + m_pImgLoginClose->GetHeight()/3);
	AddButton(rcTmp, m_pImgLoginClose, 3, ID_LOGIN_CLOSE);	//关闭按钮

	m_pImgLoginSetting = Image::FromFile(CPathFactory::GetImageLoginSettingPath());
	rcTmp.left -= m_pImgLoginSetting->GetWidth()+5;
	rcTmp.right = rcTmp.left + m_pImgLoginSetting->GetWidth();
	AddButton(rcTmp, m_pImgLoginSetting, 3, ID_LOGIN_SETTING);	//设置按钮

	rcTmp.left = 25;
	rcTmp.top = rcWnd.top + 50;
	rcTmp.right = rcTmp.left + 255;
	rcTmp.bottom = rcTmp.top + 35;
	GetDlgItem(IDC_EDIT_TRADEUSER)->MoveWindow(rcTmp);  // 用户名

	rcTmp.left = rcTmp.right + 15;
	rcTmp.right = rcTmp.left + m_pImgLoginSetting->GetWidth();
	rcTmp.top += 7;
	rcTmp.bottom = rcTmp.top + m_pImgLoginSetting->GetHeight()/3;
 	AddButton(rcTmp, m_pImgCheck, 3, ID_LOGIN_CHECK_USER);	// 记住帐号

	rcTmp.left = rcTmp.right + 5;
	rcTmp.right = rcTmp.left + 70;
	rcTmp.top += 3;
	GetDlgItem(IDC_STATIC_SAVE_USER)->MoveWindow(rcTmp);	// 记住帐号

	rcTmp.left = 25;
	rcTmp.top = rcWnd.top + 95;
	rcTmp.right = rcTmp.left + 255;
	rcTmp.bottom = rcTmp.top + 35;
	GetDlgItem(IDC_EDIT_TRADEPWD)->MoveWindow(rcTmp);  // 密码

	rcTmp.left = rcTmp.right + 15;
	rcTmp.right = rcTmp.left + m_pImgLoginSetting->GetWidth();
	rcTmp.top += 7;
	rcTmp.bottom = rcTmp.top + m_pImgLoginSetting->GetHeight()/3;
	AddButton(rcTmp, m_pImgCheck, 3, ID_LOGIN_CHECK_PUSER);	// 保护帐号
	
	rcTmp.left = rcTmp.right + 5;
	rcTmp.right = rcTmp.left + 70;
	rcTmp.top += 3;
	GetDlgItem(IDC_STATIC_PRO_USER)->MoveWindow(rcTmp);	// 保护帐号

	rcTmp.left = 25;
	rcTmp.top = rcWnd.top + 140;
	rcTmp.right = rcTmp.left + 255;
	rcTmp.bottom = rcTmp.top + 35;
	GetDlgItem(IDC_EDIT_VALIDCODE)->MoveWindow(rcTmp);  // 验证码

	rcTmp.left = rcTmp.right + 15;
	rcTmp.right = rcTmp.left + 90;
	rcTmp.bottom = rcTmp.top + 33;
	GetDlgItem(IDC_STATIC_IMAGE)->MoveWindow(rcTmp);	// 验证码
	m_rcYzm = rcTmp;

	rcTmp.left = 25;
	rcTmp.top = rcWnd.top + 190;
	rcTmp.right = rcTmp.left + 255;
	rcTmp.bottom = rcTmp.top + 35;
	m_pImgLoginConnect = Image::FromFile(CPathFactory::GetImageLoginConnectPath());
	AddButton(rcTmp, m_pImgLoginConnect, 3, ID_LOGIN_CONNECT, L"登 录");		// 登录

	rcTmp.left = rcTmp.right + 25;
	rcTmp.right = rcTmp.left + 30;
	GetDlgItem(IDC_BTN_REG)->MoveWindow(rcTmp);			// 帐号申请

	CRect rcHide = rcTmp;
	rcHide.right = rcHide.left;
	rcHide.bottom = rcHide.top;
	GetDlgItem(IDC_EDIT_HIDE)->MoveWindow(rcHide);  // 获取对话框焦点的控件

	rcTmp.left = 0;
	rcTmp.top = rcTmp.bottom + 10;
	rcTmp.right = rcWnd.right;
	rcTmp.bottom = rcTmp.top + 30;
	GetDlgItem(IDC_STATIC_PROMPT)->MoveWindow(rcTmp); // 提示

	m_EditUser.SetBitmaps(IDB_BITMAP_TRADE_USER);
	m_EditUser.SetBitmapBeginPos(CSize(5,5));
	m_EditUser.SetOwner(this);
	m_EditUser.SetButtonClickedMessageId(WM_USER_EDITWITHBUTTON_CLICKED);
	m_EditUser.SetButtonExistsAlways(FALSE);
	m_EditUser.SetTipTextColor(RGB(90, 90, 90));
	m_EditUser.SetActiveFrameColor(RGB(21, 131, 221));
	m_EditUser.SetNormalFrameColor(RGB(190, 190, 190));
	m_EditUser.SetTipText(L"您的帐号");
	//
	m_EditPassword.SetBitmaps(IDB_BITMAP_TRADE_PWD);
	m_EditPassword.SetBitmapBeginPos(CSize(5,5));
	m_EditPassword.SetOwner(this);
	m_EditPassword.SetButtonClickedMessageId(WM_USER_EDITWITHBUTTON_CLICKED);
	m_EditPassword.SetButtonExistsAlways(FALSE);
	m_EditPassword.SetTipTextColor(RGB(90, 90, 90));
	m_EditPassword.SetActiveFrameColor(RGB(21, 131, 221));
	m_EditPassword.SetNormalFrameColor(RGB(190, 190, 190));
	m_EditPassword.SetTipText(L"密码");

	m_CtrlEditValidCode.SetBitmaps(IDB_BITMAP_TRADE_YZM);
	m_CtrlEditValidCode.SetBitmapBeginPos(CSize(5,5));
	m_CtrlEditValidCode.SetOwner(this);
	m_CtrlEditValidCode.SetButtonClickedMessageId(WM_USER_EDITWITHBUTTON_CLICKED);
	m_CtrlEditValidCode.SetButtonExistsAlways(FALSE);
	m_CtrlEditValidCode.SetTipTextColor(RGB(90, 90, 90));
	m_CtrlEditValidCode.SetActiveFrameColor(RGB(21, 131, 221));
	m_CtrlEditValidCode.SetNormalFrameColor(RGB(190, 190, 190));
	m_CtrlEditValidCode.SetTipText(L"请输入验证码");
	
	LOGFONT logFont;
    //内容字体设置
    logFont.lfHeight =20;
    logFont.lfWidth = 0;
    logFont.lfEscapement = 0;
    logFont.lfOrientation = 0;
    logFont.lfWeight = FW_THIN;
    logFont.lfItalic = 0;
    logFont.lfUnderline = 0;
    logFont.lfStrikeOut = 0;
	logFont.lfCharSet = ANSI_CHARSET;
    logFont.lfOutPrecision = OUT_DEFAULT_PRECIS;
    logFont.lfClipPrecision = CLIP_DEFAULT_PRECIS;
    logFont.lfQuality = PROOF_QUALITY;
    logFont.lfPitchAndFamily = VARIABLE_PITCH | FF_ROMAN;
	_tcscpy(logFont.lfFaceName, gFontFactory.GetExistFontName(L"微软雅黑"));	//...
	
	CRect rect;
	GetDlgItem(IDC_EDIT_TRADEUSER)->GetWindowRect(&rect);
	
	//
	m_EditUser.SetTipFont(&logFont);
	m_EditPassword.SetTipFont(&logFont);
	m_CtrlEditValidCode.SetTipFont(&logFont);
	
	//
	logFont.lfHeight = 21;
	m_EditUser.SetEditFont(&logFont);
	m_EditPassword.SetEditFont(&logFont);
	m_CtrlEditValidCode.SetEditFont(&logFont);
	
	//
	if ( rect.Width() > 0)
		m_EditUser.SetEditArea(10, 5, 10, 0, rect.Width(), 30);
	else
		m_EditUser.SetEditArea(10, 5, 10, 0, 285, 35);

	if ( rect.Width() > 0)
		m_CtrlEditValidCode.SetEditArea(10, 5, 10, 0, rect.Width(), 30);
	else
		m_CtrlEditValidCode.SetEditArea(10, 5, 10, 0, 285, 30);
	
	//lint --e(569)
	m_EditPassword.SetPasswordCharEx(_T('●'));
	if ( rect.Width() > 0)
		m_EditPassword.SetEditArea(10, 5, 10, 0, rect.Width(), 30);
	else
		m_EditPassword.SetEditArea(10, 5, 10, 0, 285, 30);

	// 字体调整
	if ( m_fontStaticText.m_hObject == 0 )
	{
		LOGFONT lf = {0};
		lf.lfHeight = -12;
		_tcscpy(lf.lfFaceName, gFontFactory.GetExistFontName(L"新宋体"));	//...
		m_fontStaticText.CreateFontIndirect(&lf);
		ASSERT( m_fontStaticText.m_hObject );
	}

	COLORREF clrText = GetSysColor(COLOR_WINDOWTEXT);
	m_StaticPrompt.SetTransparentDrawText(true);
	m_StaticPrompt.SetMyTextColor(clrText);
	GetDlgItem(IDC_STATIC_PROMPT)->SetFont(GetFont());

	ShowPrompt(_T(""));

	//设置初始位置
	int cx = GetSystemMetrics(SM_CXSCREEN);
	int cy = GetSystemMetrics(SM_CYSCREEN);
	SetWindowPos(&wndBottom,cx/2-200,cy/2-130,0,0,SWP_NOSIZE); //初始位置在中间

	//获取服务器信息
	FromXml();
	LoadLoginInfo();

	if (m_bSaveUser)
	{
		m_mapBtn[ID_LOGIN_CHECK_USER].SetImage(m_pImgCheck);
	}
	else
	{
		m_mapBtn[ID_LOGIN_CHECK_USER].SetImage(m_pImgUnCheck);
	}

	if (m_bProtect)
	{
		m_mapBtn[ID_LOGIN_CHECK_PUSER].SetImage(m_pImgCheck);
	}
	else
	{
		m_mapBtn[ID_LOGIN_CHECK_PUSER].SetImage(m_pImgUnCheck);
	}

	m_CtrBtnReg.SubclassDlgItem(IDC_BTN_REG, this);
	HCURSOR cursor = ::LoadCursor(NULL, MAKEINTRESOURCE(32649));
	m_CtrBtnReg.SetCursor(cursor);
	m_CtrBtnReg.SetTextColor(RGB(185,185,185));
	m_CtrBtnReg.SetHoverTextColor(RGB(198,177,31));
	m_CtrBtnReg.SetUnderline(HS_ALWAYS);
	UpdateData(FALSE);
	
	HideControl(m_stLoginInfo.eTradeLoginType == ETT_TRADE_SIMULATE);

	return TRUE;
}

void CDlgTradeLogin::OnOK()
{
	// 登录
	if ( NULL==m_pLogin )
	{
		ASSERT( 0 );
		return;
	}

	if ( ELSLogining==m_eCurLoginStatus /*|| ELSLogined==m_eCurLoginStatus*/ )
	{
		// ing or ed，不再重新登录
		return;
	}

	if((m_stLoginInfo.eTradeLoginType != ETT_TRADE_SIMULATE) && (!GetUserInputLoginInfo()))
	{
		return ;
	}

	SetCtrlByLoginStatus(ELSLogining);
	ShowPrompt(_T("建立连接..."));
	if ( !m_pLogin->Connect(m_stLoginInfo, this) )
	{
		SetCtrlByLoginStatus(ELSNotLogin);
		return;
	}

	SaveLoginInfo();
}

void CDlgTradeLogin::SimulateTradeLogin()
{
	// 登录
	if ( NULL==m_pLogin )
	{
		ASSERT( 0 );
		return;
	}

	if ( ELSLogining==m_eCurLoginStatus )
	{
		// ing or ed，不再重新登录
		return;
	}

	// 获取代理的信息:
	T_ProxyInfo ProxyInfo;	
	bool32 bValidProxyInfo = GetProxyInfoFromXml(ProxyInfo);
	if ( ProxyInfo.uiProxyType <= 0 || ProxyInfo.uiProxyType > 3 )
	{
		// 不支持sock5??
		bValidProxyInfo = false;
	}

	if ( bValidProxyInfo && m_bUseProxy )
	{
		m_stLoginInfo.m_proxyInfol.uiProxyType = ProxyInfo.uiProxyType;
		m_stLoginInfo.m_proxyInfol.StrProxyAddress = ProxyInfo.StrProxyAddress;
		m_stLoginInfo.m_proxyInfol.uiProxyPort = ProxyInfo.uiProxyPort;
		m_stLoginInfo.m_proxyInfol.StrUserName = ProxyInfo.StrUserName;
		m_stLoginInfo.m_proxyInfol.StrUserPwd = ProxyInfo.StrUserPwd;
	}
	else
	{
		m_stLoginInfo.m_proxyInfol.uiProxyType = 0;
		m_stLoginInfo.m_proxyInfol.StrProxyAddress = L"";
		m_stLoginInfo.m_proxyInfol.uiProxyPort = 0;
		m_stLoginInfo.m_proxyInfol.StrUserName = L"";
		m_stLoginInfo.m_proxyInfol.StrUserPwd = L"";
	}
	FromXml();

	SetCtrlByLoginStatus(ELSLogining);
	ShowPrompt(_T("建立连接..."));
	if ( !m_pLogin->Connect(m_stLoginInfo, this) )
	{
		SetCtrlByLoginStatus(ELSNotLogin);
	}
}

void CDlgTradeLogin::OnCancel()
{
	// 取消
	if ( NULL==m_pLogin )
	{
		ASSERT( 0 );
		return;
	}

	if ( ELSNotLogin==m_eCurLoginStatus )
	{
		// 未登录或者
		m_pLogin->OnLoginDlgCancel();
		return;
	}

// 	if ( IDYES!=MessageBox(_T("确定取消登录？"), _T("确认"), MB_ICONQUESTION|MB_YESNO) )
// 	{
// 		return;
// 	}

	m_pLogin->StopLogin();
	// 等待回调处理
}

void CDlgTradeLogin::OnSize( UINT nType, int cx, int cy )
{
	CDialog::OnSize(nType, cx, cy);
}

void CDlgTradeLogin::InitLogin(iTradeLogin *pLogin)
{
	m_pLogin = pLogin;

	InitServerInfo();

	InitLoginInfo();

	PostMessage(WM_GETVALIDCODE_AFTERLOGIN,NULL,NULL);

	// 模拟交易直接自动登录
	if (ETT_TRADE_SIMULATE == m_stLoginInfo.eTradeLoginType)
	{
		SimulateTradeLogin();
	}
	
//	OnChangeValidCode();	// 获取验证码
// 
// 	
// 	// 测试用
//	m_EditPassword.SetWindowText(L"a123456");
// 	m_CtrlEditValidCode.SetWindowText(m_stLoginInfo.StrValidCode);
}

void CDlgTradeLogin::InitServerInfo() // 初始化服务器信息列表
{
	// 服务器内部写死（正欣写在dll中）
	m_ComboServer.InsertString(0,L"实盘环境");
	m_ComboServer.InsertString(1,L"模拟盘环境");
	m_ComboServer.SetCurSel(0);
}

void CDlgTradeLogin::InitLoginInfo()
{
	LoadLoginInfo();
	ShowPrompt(_T(""));

	if ( m_EditUser.GetSafeHwnd()!=NULL )
	{
		m_EditUser.SetWindowText(m_stLoginInfo.StrUser);
		m_EditPassword.SetWindowText(m_stLoginInfo.StrPwd);
		SetProtect(m_bProtect);

		if( m_bSaveUser )
		{
			m_mapBtn[ID_LOGIN_CHECK_USER].SetImage(m_pImgCheck);
		}
		else
		{
			m_mapBtn[ID_LOGIN_CHECK_USER].SetImage(m_pImgUnCheck);
		}

		for ( int i=0; i<m_aServer.size(); ++i )
		{
			const T_TradeServerInfo &stSer = m_aServer[i];

			if ( stSer.StrIP.CompareNoCase(m_stLoginInfo.StrServerIP)==0
				&& stSer.nPort== m_stLoginInfo.iServerPort )
			{
				m_ComboServer.SetCurSel(i);
				break;
			}
		}

		UpdateData(FALSE);
	}
}

void CDlgTradeLogin::LoadLoginInfo()
{
	if (ETT_TRADE_FIRM == m_stLoginInfo.eTradeLoginType)
	{
		CString StrV;
		m_bSaveUser = FALSE;
		if ( CEtcXmlConfig::Instance().ReadEtcConfig(KStrTradeLoginSection, KStrTradeLoginKeySaveUser, NULL, StrV) )
		{
			m_bSaveUser = _ttoi(StrV)!=0;
		}

		m_bProtect = FALSE;
		if ( CEtcXmlConfig::Instance().ReadEtcConfig(KStrTradeLoginSection, KStrTradeLoginKeySaveProtect, NULL, StrV) )
		{
			m_bProtect = _ttoi(StrV)!=0;
		}

		T_TradeLoginInfo stTmp;
		if ( m_bSaveUser )
		{
			CEtcXmlConfig::Instance().ReadEtcConfig(KStrTradeLoginSection, KStrTradeLoginKeyUser, NULL, stTmp.StrUser);
		}	
		m_stLoginInfo.StrUser = stTmp.StrUser;
		m_stLoginInfo.StrPwd = _T("");
	}
	else
	{
		m_bSaveUser = true;
		m_bProtect = true;
		CGGTongDoc* pDoc = (CGGTongDoc*)AfxGetDocument();
		m_stLoginInfo.StrUser = pDoc->m_pAbsCenterManager->GetUserName();
		m_stLoginInfo.StrPwd = pDoc->m_pAbsCenterManager->GetUserPwd();
	}

	//获取注册地址
	GetRegisterAddress();
}

void CDlgTradeLogin::SaveLoginInfo()
{
	// 实盘交易才保存登录信息
	if (ETT_TRADE_FIRM == m_stLoginInfo.eTradeLoginType)
	{
		LPCWSTR pszNULL = NULL;
		LPCWSTR pszValue = m_bSaveUser?_T("1"):_T("0");
		CEtcXmlConfig::Instance().WriteEtcConfig(KStrTradeLoginSection, KStrTradeLoginKeySaveUser, pszValue);
		pszValue = m_bSaveUser?(LPCWSTR)m_stLoginInfo.StrUser:pszNULL;
		CEtcXmlConfig::Instance().WriteEtcConfig(KStrTradeLoginSection, KStrTradeLoginKeyUser, pszValue);
		pszValue = m_bProtect?_T("1"):_T("0");
		CEtcXmlConfig::Instance().WriteEtcConfig(KStrTradeLoginSection, KStrTradeLoginKeySaveProtect, pszValue);
	}
}

//获取注册地址
void CDlgTradeLogin::GetRegisterAddress()
{	
	CString strPath = CPathFactory::GetPublicConfigPath()+_T("registerAddress.xml");
	string strPathFile;
	UnicodeToUtf8(strPath, strPathFile);
	TiXmlDocument doc;
	doc.LoadFile(strPathFile.c_str(), TIXML_DEFAULT_ENCODING);
	
	TiXmlHandle docHandle(&doc);
	TiXmlHandle rootHandle = docHandle.FirstChild("registerAddress");
	if(rootHandle.Node() == NULL)
	{
		return;
	}
	
	// address
	TiXmlElement* txeAddress = rootHandle.FirstChildElement("address").ToElement();
	if (NULL!=txeAddress)
	{
		const char* tAddress = txeAddress->GetText();
		if (NULL!=tAddress)
		{
			strncpy(m_chRegisterAddress,tAddress,sizeof(m_chRegisterAddress)-1);
		}
	}
}

void CDlgTradeLogin::OnLoginOK()
{
	ShowPrompt(_T("交易登录完成。"));
	SetCtrlByLoginStatus(ELSLogined);

	SaveLoginInfo();
}

void CDlgTradeLogin::OnLoginUserStop()
{
	// 用户取消
	SetCtrlByLoginStatus(ELSNotLogin);
}

void CDlgTradeLogin::OnLoginError( const CString &StrErr )
{
	MessageBox(StrErr,L"错误提示",MB_ICONERROR|MB_OK);
	ShowPrompt(_T("登录失败"));
	SetCtrlByLoginStatus(ELSNotLogin);
}

void CDlgTradeLogin::OnLoginStep( const CString &Str )
{
	ShowPrompt(Str);
}

void CDlgTradeLogin::OnUpdateValidCode()
{
	OnChangeValidCode();
}

void CDlgTradeLogin::ShowPrompt( const CString &Str )
{
	CWnd *pWnd = GetDlgItem(IDC_STATIC_PROMPT);
	ASSERT(pWnd);
	if (pWnd)
	{
		pWnd->SetWindowText(Str);
	}
}

void CDlgTradeLogin::SetCtrlByLoginStatus( int iLoginStatus )
{
	switch ( iLoginStatus )
	{
	case ELSNotLogin:
	case ELSLogined:		// 还原到未登录状态
		{
			m_eCurLoginStatus = (E_LoginStatus)iLoginStatus;

			m_ComboServer.EnableWindow(TRUE);
			m_EditUser.EnableWindow(TRUE);
			m_EditPassword.EnableWindow(TRUE);
			m_mapBtn[ID_LOGIN_CHECK_USER].EnableButton(TRUE);
			m_CtrlEditValidCode.EnableWindow(TRUE);
			m_mapBtn[ID_LOGIN_CHECK_PUSER].EnableButton(TRUE);
			m_EditPassword.SetWindowText(_T(""));
			m_StrValidCode = _T("");

			m_mapBtn[ID_LOGIN_CONNECT].EnableButton(TRUE,TRUE);
			GetDlgItem(IDC_BTN_REG)->EnableWindow(TRUE);

			m_StaticPrompt.SetWindowText(_T(""));
		}
		break;
	case ELSLogining:
		{
			m_eCurLoginStatus = (E_LoginStatus)iLoginStatus;

			m_ComboServer.EnableWindow(FALSE);
			m_EditUser.EnableWindow(FALSE);
			m_EditPassword.EnableWindow(FALSE);
			m_mapBtn[ID_LOGIN_CHECK_USER].EnableButton(FALSE);
			m_CtrlEditValidCode.EnableWindow(FALSE);
			m_mapBtn[ID_LOGIN_CHECK_PUSER].EnableButton(FALSE);
			
			m_mapBtn[ID_LOGIN_CONNECT].EnableButton(FALSE,TRUE);
			GetDlgItem(IDC_BTN_REG)->EnableWindow(FALSE);
		}
		break;
	default:
		//ASSERT(0);
		break;
	}
	GetDlgItem(IDC_EDIT_HIDE)->SetFocus();
	UpdateData(FALSE);
}

bool CDlgTradeLogin::GetUserInputLoginInfo()
{
	UpdateData(TRUE);
	CString strValidCode=L"";
	CString strCode = m_sValidCode;
	strCode.Remove(' ');

	m_EditUser.GetWindowText(m_stLoginInfo.StrUser);
	m_EditPassword.GetWindowText(m_stLoginInfo.StrPwd);
	m_CtrlEditValidCode.GetWindowText(strValidCode);

	if ( m_stLoginInfo.StrUser.IsEmpty() ) 
	{
		MessageBox(_T("用户名不能为空！"),_T("提示"),MB_ICONWARNING);
		m_EditUser.SetFocus();
		return false;
	}

	if ( m_stLoginInfo.StrPwd.IsEmpty() )
	{
		MessageBox(_T("密码不能为空！"),_T("提示"),MB_ICONWARNING);
		m_EditPassword.SetFocus();
		return false;
	}
	string sValidCode, sCode;
	UnicodeToUtf8(strValidCode,sValidCode);
	UnicodeToUtf8(strCode, sCode);
	if (_stricmp(sValidCode.c_str(), sCode.c_str()))
	{
		MessageBox(_T("验证码错误！"),_T("提示"),MB_ICONWARNING);
		OnChangeValidCode();
		m_CtrlEditValidCode.SetFocus();
		return false;
	}

	// 获取代理的信息:
	T_ProxyInfo ProxyInfo;	
	bool32 bValidProxyInfo = GetProxyInfoFromXml(ProxyInfo);

	if ( ProxyInfo.uiProxyType <= 0 || ProxyInfo.uiProxyType > 3 )
	{
		// 不支持sock5??
		bValidProxyInfo = false;
	}

	if ( bValidProxyInfo && m_bUseProxy )
	{
		m_stLoginInfo.m_proxyInfol.uiProxyType = ProxyInfo.uiProxyType;
		m_stLoginInfo.m_proxyInfol.StrProxyAddress = ProxyInfo.StrProxyAddress;
		m_stLoginInfo.m_proxyInfol.uiProxyPort = ProxyInfo.uiProxyPort;
		m_stLoginInfo.m_proxyInfol.StrUserName = ProxyInfo.StrUserName;
		m_stLoginInfo.m_proxyInfol.StrUserPwd = ProxyInfo.StrUserPwd;
	}
	else
	{
		m_stLoginInfo.m_proxyInfol.uiProxyType = 0;
		m_stLoginInfo.m_proxyInfol.StrProxyAddress = L"";
		m_stLoginInfo.m_proxyInfol.uiProxyPort = 0;
		m_stLoginInfo.m_proxyInfol.StrUserName = L"";
		m_stLoginInfo.m_proxyInfol.StrUserPwd = L"";
	}

// 	int iServerSel = m_ComboServer.GetCurSel();
// 	if (0==iServerSel)	// 实盘
// 	{
// 		m_stLoginInfo.iServerType = 0;
// 	}
// 	else if (1==iServerSel)	// 模拟盘
// 	{
// 		m_stLoginInfo.iServerType = 1;
// 	}
// 	else
// 	{
// 		//ASSERT(0);
// 		MessageBox(_T("请选择有效的交易服务器！"));
// 		return false;
// 	}
	FromXml();
	return true;
}

BOOL CDlgTradeLogin::OnEraseBkgnd( CDC* pDC )
{
	CDC memDC;
	memDC.CreateCompatibleDC(pDC);
	CBitmap bmp;
	CRect rcWindow;
	GetWindowRect(&rcWindow);
	rcWindow.OffsetRect(-rcWindow.left, -rcWindow.top);
	
	bmp.CreateCompatibleBitmap(pDC, rcWindow.Width(), rcWindow.Height());
	memDC.SelectObject(&bmp);
	memDC.SetBkMode(TRANSPARENT);
	Gdiplus::Graphics graphics(memDC.GetSafeHdc());

	// caption
	RectF fRect;
	fRect.X = (REAL)m_rcCaption.left;
	fRect.Y = (REAL)m_rcCaption.top;
	fRect.Width = (REAL)m_rcCaption.Width();
	fRect.Height = (REAL)m_rcCaption.Height();
	if (NULL != m_pImgCaption)
	{
		graphics.DrawImage(m_pImgCaption, fRect, 0, 0, fRect.Width, fRect.Height, UnitPixel);
	}

	DrawLogo(graphics, fRect);

	// 标题栏文本
	{
		RectF grect;
		grect.X = (REAL)25;
		grect.Y = (REAL)1;
		grect.Width = (REAL)80;
		grect.Height = (REAL)fRect.Height;
		
		//绘制文字
		StringFormat strFormat;
		strFormat.SetAlignment(StringAlignmentCenter);
		strFormat.SetLineAlignment(StringAlignmentCenter);

		typedef struct T_NcFont 
		{
		public:
			CString	m_StrName;
			float   m_Size;
			int32	m_iStyle;
			
		}T_NcFont;
		T_NcFont m_Font;
		m_Font.m_StrName = gFontFactory.GetExistFontName(L"微软雅黑");//...
		m_Font.m_Size	 = 10;
		m_Font.m_iStyle	 = FontStyleRegular;	
		Gdiplus::FontFamily fontFamily(m_Font.m_StrName);
		Gdiplus::Font font(&fontFamily, m_Font.m_Size, m_Font.m_iStyle, UnitPoint);

		RectF rcBound;
		PointF point;
		graphics.MeasureString(m_strAppName, m_strAppName.GetLength(), &font, point, &strFormat, &rcBound);
		grect.Width = rcBound.Width;
		
		SolidBrush brush((ARGB)Color::White);
		brush.SetColor(Color::Color(255,255,255));
		graphics.DrawString(m_strAppName, m_strAppName.GetLength(), &font, grect, &strFormat, &brush);
	}

	// bg
	fRect.X = (REAL)m_rcBg.left;
	fRect.Y = (REAL)m_rcBg.top;
	fRect.Width = (REAL)m_rcBg.Width();
	fRect.Height = (REAL)m_rcBg.Height();
	if (NULL != m_pImgBg)
	{
		graphics.DrawImage(m_pImgBg, fRect, 0, 0, (REAL)m_pImgBg->GetWidth(), (REAL)m_pImgBg->GetHeight(), UnitPixel);
	}
	
	CRect rcPaint;
	pDC->GetClipBox(&rcPaint);
	map<int, CNCButton>::iterator iter;
	CRect rcControl;
	
	for (iter=m_mapBtn.begin(); iter!=m_mapBtn.end(); ++iter)
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
	
	pDC->BitBlt(0, 0, rcWindow.Width(), rcWindow.Height(), &memDC, 0, 0, SRCCOPY);
	pDC->SelectClipRgn(NULL);

	memDC.DeleteDC();
	bmp.DeleteObject();
	return TRUE; 
}

void CDlgTradeLogin::OnChangeValidCode()
{
	if (ELSLogining==m_eCurLoginStatus)
	{
		return;
	}
  	GetValidCode();	// 获取验证码
	InvalidateRect(m_rcYzm);
}

// 获取验证码
void CDlgTradeLogin::GetValidCode()
{
	m_sValidCode = L"";
	
	if(m_stLoginInfo.eTradeLoginType == ETT_TRADE_SIMULATE)
	{
		return;
	}

    const char ch[64]="JaAbxyCz1K2iXjWVklBoUp35TImSqLrMsHtuYv6FGw4nE7N3cOdPefgZh8Q90DR";
    srand((unsigned)time(NULL));
    for(int i=0;i<4;i++)
	{
		m_sValidCode += ch[rand()%63];
 		m_sValidCode +=L" ";
    }
}

void CDlgTradeLogin::OnPaint()
{
	CPaintDC dc(this);

	CWnd* pWnd = GetDlgItem(IDC_STATIC_IMAGE);
	if (NULL == pWnd)
	{
		return;
	}

	if (L"" == m_sValidCode)
	{
		GetValidCode();
	}
	
	if (L"" != m_sValidCode)
	{
		CString strFontName = gFontFactory.GetExistFontName(L"微软雅黑");
		//每个字符的位置随机偏移4
		CRect r(0,0,0,0);
		dc.DrawText(m_sValidCode,&r,DT_CALCRECT);
		const int w = r.Width()+4;
		const int iCharWidth = w*2/m_sValidCode.GetLength()+5;

		::SetGraphicsMode(dc.m_hDC,GM_ADVANCED);//为字体倾斜作准备
		for(int j = 0 ; j < m_sValidCode.GetLength()/2 ; j++)
		{
			//设置字体
			CFont* pFont = dc.GetCurrentFont();
			LOGFONT logFont;
			pFont->GetLogFont(&logFont);

			logFont.lfHeight =-21;
			logFont.lfWidth = 0;
			logFont.lfEscapement = 0;
			logFont.lfWeight = FW_NORMAL;
			logFont.lfItalic = 0;
			logFont.lfUnderline = 0;
			logFont.lfStrikeOut = 0;
			logFont.lfCharSet = ANSI_CHARSET;
			logFont.lfClipPrecision = CLIP_DEFAULT_PRECIS;
			logFont.lfQuality = PROOF_QUALITY;
			logFont.lfPitchAndFamily = VARIABLE_PITCH | FF_ROMAN;
			_tcscpy(logFont.lfFaceName, strFontName);///
			logFont.lfOutPrecision = OUT_TT_ONLY_PRECIS;
			logFont.lfOrientation = rand()%90;
			CFont font;
			font.CreateFontIndirect(&logFont);
			dc.SelectObject(&font);
			
			int x = iCharWidth*j+rand()%5+m_rcYzm.left;
			int y = rand()%5+m_rcYzm.top;
			CString str = m_sValidCode.Mid(j*2,2);
			dc.SetBkMode(TRANSPARENT);
			dc.SetTextColor(RGB(0x4d,0x4d,0x4d));
			dc.TextOut(x, y, str);
			font.DeleteObject();
		}
	}
} 

void CDlgTradeLogin::OnButtonApply()	// 申请账号链接
{
	CGGTongApp* pApp = (CGGTongApp*)AfxGetApp();
	if (NULL == pApp)
	{
		return;
	}

	bool32 bShowIE = FALSE;
	int32 iWidth = 1024, iHeight = 750;
	int32 iSize = pApp->m_pConfigInfo->m_aWndSize.GetSize();
	CString StrName = L"";
	for (int32 i=0; i<iSize; i++)
	{
		CWndSize wnd = pApp->m_pConfigInfo->m_aWndSize[i];
		if (L"开户" == wnd.m_strID)
		{
			StrName = wnd.m_strTitle;
			iWidth = wnd.m_iWidth;
			iHeight = wnd.m_iHeight;

			if (0==iWidth || 0== iHeight)
			{
				bShowIE = TRUE;
			}

			break;
		}
	}

	if (StrName.IsEmpty())
	{
		StrName = pApp->m_pConfigInfo->m_StrBtName;
	}
	
	UrlParser urlParser(L"alias://openaccount");
	if (bShowIE)
	{
		ShellExecute(0, L"open", urlParser.GetUrl(), NULL, NULL, SW_NORMAL);
	}
	else
	{
		CDlgTodayCommentIE::ShowDlgIEWithSize(StrName,urlParser.GetUrl(), CRect(0, 0, iWidth, iHeight));
	}
}

void CDlgTradeLogin::OnButtonSetPro()	// 设置网络
{
	UpdateData(true);
//	CGGTongApp *pApp  =  (CGGTongApp*) AfxGetApp();

	CGGTongDoc *pDoc = AfxGetDocument();
	CString StrUserName = _T("");
	if ( pDoc != NULL )
	{
		StrUserName = pDoc->m_pAbsCenterManager->GetUserName();
	}

// 	if (n_dlgNetWork.m_hWnd)
// 	{
// 		n_dlgNetWork.ShowWindow(SW_SHOW);
// 		n_dlgNetWork.SetIinitDailog(StrUserName);
// 	}
	CDlgNetWorkSet dlg;
	dlg.SetIinitDailog(StrUserName);
	int nResult = dlg.DoModal();
	m_iLinkID = dlg.GetNetSelectIndex();

	if(IDOK == nResult)	// 如果更改了网络设置，则重新获取一下验证码
	{
		if(m_pImageValidCode != NULL)
		{
			delete m_pImageValidCode;
			m_pImageValidCode = NULL;
		}
		OnChangeValidCode();
	}
}

void CDlgTradeLogin::OnBtnProtect()
{
	if (m_bProtect)
	{
		m_bProtect = FALSE;
	}
	else
	{
		m_bProtect = TRUE;
	}
	SetProtect(m_bProtect);
}

void CDlgTradeLogin::SetProtect(BOOL bProtect)
{
	if (bProtect)
	{
		m_EditUser.SetPasswordChar('#');
	}
	else
	{
		m_EditUser.SetPasswordChar(NULL);
	}
	
	if (m_bProtect)
	{
		m_mapBtn[ID_LOGIN_CHECK_PUSER].SetImage(m_pImgCheck);
	}
	else
	{
		m_mapBtn[ID_LOGIN_CHECK_PUSER].SetImage(m_pImgUnCheck);
	}

	m_EditUser.Invalidate();
}

LRESULT CDlgTradeLogin::OnNcHitTest( CPoint point )
{
	map<int, CNCButton>::iterator iter;
	ScreenToClient(&point);
	
	// 遍历所有按钮
	for (iter=m_mapBtn.begin(); iter!=m_mapBtn.end(); ++iter)
	{
		CNCButton &btnControl = iter->second;
		
		// 点point是否在已绘制的按钮区域内
		if (btnControl.PtInButton(point))
		{
			return HTCLIENT;
		}
	}
	
	return HTCAPTION;
}

BOOL CDlgTradeLogin::PreTranslateMessage(MSG* pMsg) 
{
	if ( WM_KEYDOWN == pMsg->message && VK_RETURN == pMsg->wParam)
	{
		OnCommand(ID_LOGIN_CONNECT, 0);
		return TRUE;
	}
	
	return BaseDialog::PreTranslateMessage(pMsg);
}

void CDlgTradeLogin::OnEnChangeEditTradeUser()
{
	m_EditPassword.SetWindowText(_T(""));
	m_CtrlEditValidCode.SetWindowText(_T(""));
	UpdateData(FALSE);
}

bool CDlgTradeLogin::FromXml()
{	
	CString StrPath  = CPathFactory::GetServerInfoPath();
	if ( StrPath.IsEmpty() )
	{
		return FALSE;
	}

	string sPath = _Unicode2MultiChar(StrPath);
	const char* StrFilePath = sPath.c_str();

	TiXmlDocument myDocument = TiXmlDocument(StrFilePath);
	
	if ( !myDocument.LoadFile())
	{
		return FALSE;
	}
	
	// XMLDATA
	TiXmlElement* pRootElement = myDocument.RootElement();
	if ( NULL == pRootElement )
	{
		return FALSE;
	}

	// 实盘交易服务器信息
	if (ETT_TRADE_FIRM == m_stLoginInfo.eTradeLoginType)
	{
		TiXmlElement* pNetWorkSet = pRootElement->FirstChildElement(KStrElementAttriNetWorkSet);
		if( NULL != pNetWorkSet && NULL != pNetWorkSet->Value()  )
		{
			ASSERT( 0 == strcmp(pNetWorkSet->Value(), KStrElementAttriNetWorkSet) );

			TiXmlElement* pNetWork = pNetWorkSet->FirstChildElement(KStrElementAttriNetWork);
			for (int i=0 ; NULL!=pNetWork ; pNetWork = pNetWork->NextSiblingElement(KStrElementAttriNetWork) )
			{
				//selected 
				const char* strSelected = pNetWork->Attribute(KStrElementAttriNetWorkSelected);
				//获取选中的服务器index
				if (0!=atoi(strSelected))
				{
					m_iLinkID = i;
					// 名称:
					const char* StrName = pNetWork->Attribute(KStrElementAttriNetWorkName);

					// IP:
					const char* StrIp = pNetWork->Attribute(KStrElementAttriNetWorkURL);

					// 端口:
					const char* StrPort = pNetWork->Attribute(KStrElementAttriNetWorkPort);

					// 转换编码:
					TCHAR TStrHostName[1024];
					memset(TStrHostName, 0, sizeof(TStrHostName));
					MultiCharCoding2Unicode(EMCCUtf8, StrName, strlen(StrName), TStrHostName, sizeof(TStrHostName) / sizeof(TCHAR));

					//
					TCHAR TStrHostAdd[1024];
					memset(TStrHostAdd, 0, sizeof(TStrHostAdd));
					MultiCharCoding2Unicode(EMCCUtf8, StrIp, strlen(StrIp), TStrHostAdd, sizeof(TStrHostAdd) / sizeof(TCHAR));

					//
					m_stLoginInfo.StrServerName	= TStrHostName;
					m_stLoginInfo.StrServerIP	= TStrHostAdd;
					m_stLoginInfo.iServerPort	= (UINT)atoi(StrPort);
					break;
				}

				i++;			
			}
		}
	}
	else // 模拟交易服务器信息
	{
		TiXmlElement* pNetWorkSet = pRootElement->FirstChildElement(KStrElementAttriSimulateTrade);
		if( NULL != pNetWorkSet && NULL != pNetWorkSet->Value()  )
		{
			ASSERT( 0 == strcmp(pNetWorkSet->Value(), KStrElementAttriSimulateTrade) );

			TiXmlElement* pNetWork = pNetWorkSet->FirstChildElement(KStrElementAttriNetWork);
			for (int i=0 ; NULL!=pNetWork ; pNetWork = pNetWork->NextSiblingElement(KStrElementAttriNetWork) )
			{
				// 暂时取第一个服务器	
				m_iLinkID = i;
				// 名称:
				const char* StrName = pNetWork->Attribute(KStrElementAttriNetWorkName);

				// IP:
				const char* StrIp = pNetWork->Attribute(KStrElementAttriNetWorkURL);

				// 端口:
				const char* StrPort = pNetWork->Attribute(KStrElementAttriNetWorkPort);

				// 转换编码:
				TCHAR TStrHostName[1024];
				memset(TStrHostName, 0, sizeof(TStrHostName));
				MultiCharCoding2Unicode(EMCCUtf8, StrName, strlen(StrName), TStrHostName, sizeof(TStrHostName) / sizeof(TCHAR));

				//
				TCHAR TStrHostAdd[1024];
				memset(TStrHostAdd, 0, sizeof(TStrHostAdd));
				MultiCharCoding2Unicode(EMCCUtf8, StrIp, strlen(StrIp), TStrHostAdd, sizeof(TStrHostAdd) / sizeof(TCHAR));

				//
				m_stLoginInfo.StrServerName	= TStrHostName;
				m_stLoginInfo.StrServerIP	= TStrHostAdd;
				m_stLoginInfo.iServerPort	= (UINT)atoi(StrPort);
				break;
			}
		}
	}
	
	return true;
}

LRESULT CDlgTradeLogin::OnMsgGetValidcode( WPARAM w, LPARAM l )
{
	OnChangeValidCode();

	return 1;
}

bool32 CDlgTradeLogin::GetProxyInfoFromXml(OUT T_ProxyInfo& ProxyInfo)
{
	m_bUseProxy = false;
	CString strUser;
	m_EditUser.GetWindowText(strUser);
	
	// 从私人目录下读取代理信息:
	if ( strUser.GetLength() <= 0 )
	{
		return false;
	}
	
	//
	UpdateData(true);
	
	CString StrPath = CPathFactory::GetProxyInfoPath(strUser);
	

	string sPath = _Unicode2MultiChar(StrPath);
	const char* StrProxyPath = sPath.c_str();
	//	
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
	ProxyInfo.uiProxyType	  = atoi(StrProxyType);
	ProxyInfo.StrProxyAddress = StrProxyAddress;
	ProxyInfo.uiProxyPort	  = atoi(StrProxyPort);
	ProxyInfo.StrUserName	  = _A2W(StrProxyUserName);
	ProxyInfo.StrUserPwd	  = _A2W(StrProxyUserPwd);
	
	m_bUseProxy				  = atoi(StrUseProxy);
	
	return true;
}

void CDlgTradeLogin::AddButton(LPRECT lpRect, Image *pImage, UINT nCount, UINT nID, LPCTSTR lpszCaption)
{
	ASSERT(pImage);
	CNCButton btnControl; 
	btnControl.CreateButton(lpszCaption, lpRect, this, pImage, nCount, nID);
	btnControl.SetParentFocus(FALSE);
	m_mapBtn[nID] = btnControl;
}

int	 CDlgTradeLogin::TButtonHitTest(CPoint point)
{
	map<int, CNCButton>::iterator iter;
	
	// 遍历所有按钮
	for (iter=m_mapBtn.begin(); iter!=m_mapBtn.end(); ++iter)
	{
		CNCButton &btnControl = iter->second;
		
		// 点point是否在已绘制的按钮区域内
		if (btnControl.PtInButton(point) && btnControl.IsEnable())
		{
			return btnControl.GetControlId();
		}
	}
	
	return INVALID_ID;
}

void CDlgTradeLogin::OnMouseMove(UINT nFlags, CPoint point) 
{
	int32 iButton = TButtonHitTest(point);
	
	if (iButton != m_iXButtonHovering)
	{
		if (INVALID_ID != m_iXButtonHovering)
		{
			if (ID_LOGIN_CLOSE==iButton || ID_LOGIN_SETTING==iButton)
			{
				m_mapBtn[m_iXButtonHovering].MouseLeave(FALSE);
				DrawTitleBar();
			}
			else
			{
				m_mapBtn[m_iXButtonHovering].MouseLeave();
			}
			m_iXButtonHovering = INVALID_ID;
		}
		
		if (INVALID_ID != iButton)
		{	
			m_iXButtonHovering = iButton;
			if (ID_LOGIN_CLOSE==iButton || ID_LOGIN_SETTING==iButton)
			{
				m_mapBtn[m_iXButtonHovering].MouseHover(FALSE);
				DrawTitleBar();
			}
			else
			{
				m_mapBtn[m_iXButtonHovering].MouseHover();
			}
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

LRESULT CDlgTradeLogin::OnMouseLeave(WPARAM wParam, LPARAM lParam)       
{     
	if (INVALID_ID != m_iXButtonHovering)
	{
		if (ID_LOGIN_CLOSE==m_iXButtonHovering || ID_LOGIN_SETTING==m_iXButtonHovering)
		{
			m_mapBtn[m_iXButtonHovering].MouseLeave(FALSE);
			DrawTitleBar();
		}
		else
		{
			m_mapBtn[m_iXButtonHovering].MouseLeave();
		}
		m_iXButtonHovering = INVALID_ID;
	}
	
	return 0;       
} 

void CDlgTradeLogin::OnLButtonDown(UINT nFlags, CPoint point) 
{
	int32 iButton = TButtonHitTest(point);
	if (INVALID_ID != iButton)
	{
		if (ID_LOGIN_CLOSE==iButton || ID_LOGIN_SETTING==iButton)
		{
			m_mapBtn[iButton].LButtonDown(FALSE);
			DrawTitleBar();
		}
		else
		{
			m_mapBtn[iButton].LButtonDown();
		}
	}
}

void CDlgTradeLogin::OnLButtonUp(UINT nFlags, CPoint point) 
{
	int32 iButton = TButtonHitTest(point);
	if (INVALID_ID != iButton)
	{
		if (ID_LOGIN_CLOSE==iButton || ID_LOGIN_SETTING==iButton)
		{
			m_mapBtn[iButton].LButtonUp(FALSE);
			DrawTitleBar();
		}
		else
		{
			m_mapBtn[iButton].LButtonUp();
		}
	}
}

void CDlgTradeLogin::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	OnLButtonDown(nFlags, point);
	CDialog::OnLButtonDblClk(nFlags, point);
}

BOOL CDlgTradeLogin::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	int32 iID = (int32)wParam;
	std::map<int, CNCButton>::iterator it = m_mapBtn.find(iID);
	if (m_mapBtn.end() != it)
	{
		switch (iID)
		{
		case ID_LOGIN_SETTING:
			{	
				// 设置
				OnButtonSetPro();
				break;
			}
		case ID_LOGIN_CLOSE:
			{	
				// 关闭
				OnCancel();
				break;
			}
		case ID_LOGIN_CONNECT:
			{	
				OnOK();
				break;
			}
		case ID_LOGIN_CHECK_USER:
			{	
				m_bSaveUser = !m_bSaveUser;
				if( m_bSaveUser )
				{
					m_mapBtn[iID].SetImage(m_pImgCheck);
				}
				else
				{
					m_mapBtn[iID].SetImage(m_pImgUnCheck);
				}
				
				break;
			}
		case ID_LOGIN_CHECK_PUSER:
			{	
				OnBtnProtect();
				break;
			}
		default:
			{
			}
			break;
		}
	}
	
	return CDialog::OnCommand(wParam, lParam);
}

void CDlgTradeLogin::DrawLogo(Gdiplus::Graphics &graphics, const RectF &rcF)
{
	if(NULL != m_pImgLogo)
	{
		RectF destRect;
		int32 iAdjust = 6;
		destRect.X = rcF.X + iAdjust / 2.0 + 1;
		destRect.Y = rcF.Y + iAdjust;
		destRect.Height = rcF.Height - iAdjust * 2;
		destRect.Width = destRect.Height;
		graphics.DrawImage(m_pImgLogo, destRect);
	}
}

void CDlgTradeLogin::DrawTitleBar()
{
	CWindowDC dc(this);
	CDC memDC;
	memDC.CreateCompatibleDC(&dc);
	CBitmap bmp;
	
	bmp.CreateCompatibleBitmap(&dc, m_rcCaption.Width(), m_rcCaption.Height());
	memDC.SelectObject(&bmp);
	memDC.SetBkMode(TRANSPARENT);
	Gdiplus::Graphics graphics(memDC.GetSafeHdc());
	
	// caption
	RectF fRect;
	fRect.X = (REAL)m_rcCaption.left;
	fRect.Y = (REAL)m_rcCaption.top;
	fRect.Width = (REAL)m_rcCaption.Width();
	fRect.Height = (REAL)m_rcCaption.Height();
	if (NULL != m_pImgCaption)
	{
		graphics.DrawImage(m_pImgCaption, fRect, 0, 0, fRect.Width, fRect.Height, UnitPixel);
	}
	
	DrawLogo(graphics, fRect);
	
	// 标题栏文本
	{
		RectF grect;
		grect.X = (REAL)25;
		grect.Y = (REAL)1;
		grect.Width = (REAL)80;
		grect.Height = (REAL)fRect.Height;
		
		//绘制文字
		StringFormat strFormat;
		strFormat.SetAlignment(StringAlignmentCenter);
		strFormat.SetLineAlignment(StringAlignmentCenter);
		
		typedef struct T_NcFont 
		{
		public:
			CString	m_StrName;
			float   m_Size;
			int32	m_iStyle;
			
		}T_NcFont;
		T_NcFont m_Font;
		m_Font.m_StrName = gFontFactory.GetExistFontName(L"微软雅黑");	//...
		m_Font.m_Size	 = 10;
		m_Font.m_iStyle	 = FontStyleRegular;	
		Gdiplus::FontFamily fontFamily(m_Font.m_StrName);
		Gdiplus::Font font(&fontFamily, m_Font.m_Size, m_Font.m_iStyle, UnitPoint);
		
		RectF rcBound;
		PointF point;
		graphics.MeasureString(m_strAppName, m_strAppName.GetLength(), &font, point, &strFormat, &rcBound);
		grect.Width = rcBound.Width;
		
		SolidBrush brush((ARGB)Color::White);
		brush.SetColor(Color::Color(255,255,255));
		graphics.DrawString(m_strAppName, m_strAppName.GetLength(), &font, grect, &strFormat, &brush);
	}
	
	CRect rcPaint;
	dc.GetClipBox(&rcPaint);
	map<int, CNCButton>::iterator iter;
	CRect rcControl;
	
	for (iter=m_mapBtn.begin(); iter!=m_mapBtn.end(); ++iter)
	{
		CNCButton &btnControl = iter->second;
		btnControl.GetRect(rcControl);
		
		UINT ID = btnControl.GetControlId();
		
		// 判断当前按钮是否需要重绘
		if (!rcPaint.IsRectEmpty() && !CRect().IntersectRect(&rcControl, rcPaint))
		{
			continue;
		}
		
		if (ID_LOGIN_CLOSE==ID || ID_LOGIN_SETTING==ID)
		{
			btnControl.DrawButton(&graphics);
		}
	}
	
	dc.BitBlt(m_rcCaption.left, m_rcCaption.top, m_rcCaption.Width(), m_rcCaption.Height(), &memDC, 0, 0, SRCCOPY);
	dc.SelectClipRgn(NULL);
	memDC.DeleteDC();
	bmp.DeleteObject();
}

HBRUSH CDlgTradeLogin::OnCtlColor( CDC* pDC, CWnd* pWnd, UINT nCtlColor )
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	
	if (nCtlColor == CTLCOLOR_STATIC)
	{	
		CString s;
		pWnd->GetWindowText(s);
		
		pDC->SetBkMode(TRANSPARENT);
		pDC->SetTextColor(RGB(185,185,185));
		//pDC->SelectObject(&m_fontStaticText);
		
		int32 iID = pWnd->GetDlgCtrlID();
		if (IDC_EDIT_TRADEUSER!=iID && IDC_EDIT_TRADEPWD!=iID && IDC_EDIT_VALIDCODE!=iID)
		{
			return (HBRUSH)GetStockObject(NULL_BRUSH);
		}
	}

	return hbr;
}

void CDlgTradeLogin::HideControl(bool isSimulate)
{
	m_strAppName = isSimulate ? L"模拟交易" : L"实盘交易";
	GetDlgItem(IDC_STATIC_SAVE_USER)->ShowWindow(isSimulate ? SW_HIDE : SW_SHOW);
	GetDlgItem(IDC_STATIC_PRO_USER)->ShowWindow(isSimulate ? SW_HIDE : SW_SHOW);
	GetDlgItem(IDC_EDIT_TRADEUSER)->ShowWindow(isSimulate ? SW_HIDE : SW_SHOW);
	GetDlgItem(IDC_EDIT_TRADEPWD)->ShowWindow(isSimulate ? SW_HIDE : SW_SHOW);
	GetDlgItem(IDC_EDIT_VALIDCODE)->ShowWindow(isSimulate ? SW_HIDE : SW_SHOW);
	GetDlgItem(IDC_STATIC_IMAGE)->ShowWindow(isSimulate ? SW_HIDE : SW_SHOW);
	m_CtrBtnReg.ShowWindow(isSimulate ? SW_HIDE : SW_SHOW);
	m_mapBtn[ID_LOGIN_CHECK_USER].SetVisiable(isSimulate ? SW_HIDE : SW_SHOW);
	m_mapBtn[ID_LOGIN_CHECK_PUSER].SetVisiable(isSimulate ? SW_HIDE : SW_SHOW);
	m_mapBtn[ID_LOGIN_SETTING].SetVisiable(isSimulate ? SW_HIDE : SW_SHOW);

	if(isSimulate)
	{
		CRect rcTmp, rcWnd;
		GetClientRect(&rcWnd);
		rcTmp.left = 3;
		rcTmp.right = rcWnd.right;
		rcTmp.top = (int32)((rcWnd.Height() - 20) * 0.5);
		rcTmp.bottom = rcTmp.top + 20;
		m_StaticPrompt.MoveWindow(rcTmp);

		m_mapBtn[ID_LOGIN_CONNECT].GetRect(rcTmp);
		int nW = rcTmp.Width();
		rcTmp.left = (int32)((rcWnd.Width() - nW) * 0.5);
		rcTmp.right = rcTmp.left + nW;
		m_mapBtn[ID_LOGIN_CONNECT].SetRect(rcTmp);
	}
}
