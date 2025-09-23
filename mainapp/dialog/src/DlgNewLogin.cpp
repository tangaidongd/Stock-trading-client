 // DlgNewLogin.cpp : implementation file
//
#include "stdafx.h"
#include "dlgconnectset.h"
#include "DlgNewLogin.h"
#include "ShareFun.h"
#include "proxy_auth_client_base.h"
#include "pathfactory.h"
#include "tinyxml.h"
#include "IoViewBase.h"
#include "CCodeFile.h"
#include "DlgTrace.h"
#include "OptimizeServer.h"
#include "dlgconnectset.h"
#include "FontFactory.h"
#include "ColorStep.h"
#include "DlgStatic.h"
#include "DlgCheckBtn.h"
#include "coding.h"
#include "ConfigInfo.h"
#include "TraceLog.h"
#include "DlgTodayComment.h"
#include "DlgWelcome.h"
#include "FontFactory.h"
#include "PluginFuncRight.h"
#include "dlgmanual.h"
#include "md5.h"

using namespace auth;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgNewLogin dialog
#define INVALID_ID			       -1
#define ID_LOGIN_SETTING	       10000
#define ID_LOGIN_CLOSE			   10001 
#define ID_LOGIN_CONNECT		   10002
#define ID_LOGIN_CANCEL		       10003
#define ID_LOGIN_CHECK_PWD         10004
#define ID_LOGIN_CHECK_AUTOLOGIN   10005
#define ID_LOGIN_THIRDPARTY_QQ     10006
#define ID_LOGIN_THIRDPARTY_WECHAT 10007
#define ID_LOGIN_REGISTERED		   10008

#define FONT_COLOR		RGB(255,255,255)

const int32 KPasswdMaxLength		= 20;		// 用户密码最多可以输入的字符数    
const int32 KAccountMaxLength		= 50;		// 用户账户最多可以输入的字符数  
const TCHAR KFontFamily[]			= L"微软雅黑";		//对话框中文本字体
const int32 KGdiFontSize			=17;				//对话框中文本字体高度(适应于gdi)
const float KGdiPlusFontSize		=11.0f;				//对话框中文本字体高度（适应于gdi+）
const int32 KMidSpace				= 32;				//对话框左侧布局与右侧布局中间间距值
const int32 KMidSpaceLogo			= 10;				//Logo与左侧边框的间距
PROXYTYPE MyNewShowProxyTypeToAuthProxyType(UINT uMyType);

const int KRightStartPosition  = 458;

CDlgNewLogin::CDlgNewLogin(CAbsLoginDlgObserver &LoginDlgObserver, CWnd* pParent /*=NULL*/)
	: CDialog(CDlgNewLogin::IDD, pParent),
	m_LoginDlgObserver(LoginDlgObserver),
	m_CtrlStaticPrompt(*new CDlgStatic()),
	m_CtrlStaticPrompt2(*new CDlgStatic())
{
	m_StrUserName  = _T("");
	m_StrPassword  = _T("");
	m_strSelService = _T("");
	
	m_bNewUser		= false;
	m_bUseProxy		= false;
	m_bGreenChannel = false;
	m_bLogInError	= false;
	m_bCancelAuth	= false;
	m_bSavePwd		= true;
	m_bSaveUser     = true;
	
	m_nAutoLoginTimer = 0;
	m_envIndex		= 0;
	m_iIndexCurAuth = 0;
	m_iHostIndex	= 0;
	m_indexServerSel = 0;
	m_bFirstTime	= true;
	m_bCancelAuth	= false;
	m_bManualOptimaze = false;
	m_bShowWeb		= false;
	m_bDefaultUser  = FALSE;
	m_bAutoLogin    = FALSE;
	m_bShowSCToolBar= FALSE;

	m_iXButtonHovering = INVALID_ID;
	m_pImgCaption = NULL;
	m_pImgLogo = NULL;
	m_pImgTop = NULL;
	m_pImgBottom = NULL;
	m_pImgCheck = NULL;
	m_pImgUnCheck = NULL;
	m_pImgLoginClose   = NULL;
	m_pImgLoginConnect = NULL;
	m_pImgRegister     = NULL;
	m_pImgLoginSetting = NULL;
	m_pImgLoginQQ      = NULL;
	m_pImgLoginWechat  = NULL;

	m_pDlgComment = NULL;		
	m_CtrlStaticPrompt.SetTransparentDrawText(true);
	COptimizeServer::Instance()->SetOwner(GetSafeHwnd());
}

CDlgNewLogin::~CDlgNewLogin()
{
	::WritePrivateProfileString(L"TRACE2", L"HWND",	L"0", L"./trace.ini");
	
	_MYTRACE(L"=============== 登录追踪结束！==================!");

	DEL(m_pDlgComment);
	m_fontStaticText.DeleteObject();
	m_fontCheckText.DeleteObject();
	
    DEL(m_pImgCaption);
	DEL(m_pImgLogo);
	DEL(m_pImgTop);
	DEL(m_pImgBottom);
	DEL(m_pImgCheck);
	DEL(m_pImgUnCheck);

	DEL(m_pImgLoginClose);
	DEL(m_pImgLoginConnect);
	DEL(m_pImgRegister);
	DEL(m_pImgLoginSetting);
	DEL(m_pImgLoginQQ);
	DEL(m_pImgLoginWechat);
}

void CDlgNewLogin::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgNewLogin)
	DDX_Control(pDX, IDC_STATIC_PROMPT, m_CtrlStaticPrompt);
	DDX_Control(pDX, IDC_STATIC_PROMPT2, m_CtrlStaticPrompt2);
	DDX_Control(pDX, IDC_EDIT_PWD, m_CtrlEditPassword);
	DDX_Control(pDX, IDC_EDIT_USER, m_CtrlEditName);
	DDX_Control(pDX, IDC_PROGRESS_LOGIN, m_CtrlProgressLogin);
	DDX_Control(pDX, IDC_BTN_REG, m_CtrBtnReg);
	DDX_Control(pDX, IDC_BTN_VISITOR_LOGIN, m_CtrBtnVisitor);
	DDX_Control(pDX, IDC_BUTTON_OFFLINE, m_CtrBtnOffline);
    DDX_Control(pDX, IDC_BTN_PASSWORD_RETAKE, m_CtrBtnPasswordRetake);
	DDX_Text(pDX, IDC_EDIT_USER, m_StrUserName);
	DDX_Text(pDX, IDC_EDIT_PWD, m_StrPassword);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDlgNewLogin, CDialog)
	//{{AFX_MSG_MAP(CDlgNewLogin)
	// 行情
	ON_MESSAGE(UM_ViewData_OnAuthSuccess, OnMsgViewDataOnAuthSuccess)
	ON_MESSAGE(UM_ViewData_OnAuthSuccessTransToMain, OnMsgViewDataAuthSuccessTransToMain)
	ON_MESSAGE(UM_ViewData_OnConfigSuccessTransToMain, OnMsgViewDataConfigSuccessTransToMain)
	ON_MESSAGE(UM_ViewData_OnAuthFail, OnMsgViewDataOnAuthFail)
	ON_MESSAGE(UM_ViewData_OnMessage, OnMsgViewDataOnMessage)
	ON_MESSAGE(UM_ViewData_OnAllMarketInitializeSuccess, OnMsgViewDataOnAllMarketInitializeSuccess)
	ON_MESSAGE(UM_ViewData_OnDataServiceConnected, OnMsgViewDataOnDataServiceConnected)
	ON_MESSAGE(UM_ViewData_OnDataServiceDisconnected, OnMsgViewDataOnDataServiceDisconnected)
	ON_MESSAGE(UM_ViewData_OnDataRequestTimeOut, OnMsgViewDataOnDataRequestTimeOut)
	ON_MESSAGE(UM_ViewData_OnDataCommResponse, OnMsgViewDataOnDataCommResponse)
	ON_MESSAGE(UM_ViewData_OnDataServerConnected, OnMsgViewDataOnDataServerConnected)
	ON_MESSAGE(UM_ViewData_OnDataServerDisconnected, OnMsgViewDataOnDataServerDisconnected)
	ON_MESSAGE(UM_ViewData_OnDataServerLongTimeNoRecvData, OnMsgViewDataOnDataServerLongTimeNoRecvData)
	ON_MESSAGE(UM_ViewData_OnBlockFileTraslateTimeout, OnMsgViewDataOnBlockFileTranslateTimeout)
	ON_MESSAGE(UM_MainFrame_ManualOptimize,OnMsgMainFrameManualOptimize)
	ON_MESSAGE(UM_ThirdLoginRsp, OnWebThirdLoginRsp)
	ON_MESSAGE(UM_CLOSE_DLG, OnWebCloseDlg)
	
	//
	ON_MESSAGE(0x999, OnMsgDllTraceLog)
	ON_WM_TIMER()
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_MOUSEMOVE()
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave) 
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_NCHITTEST()
	ON_WM_CTLCOLOR()
	ON_WM_LBUTTONDBLCLK()

	ON_BN_CLICKED(IDC_BTN_REG, OnButtonReg)
	ON_BN_CLICKED(IDC_BTN_VISITOR_LOGIN, OnButtonVisitor)
	ON_BN_CLICKED(IDC_BUTTON_OFFLINE, OnButtonOffLine)
    ON_BN_CLICKED(IDC_BTN_PASSWORD_RETAKE, &CDlgNewLogin::OnBnClickedBtnPasswordRetake)
	//}}AFX_MSG_MA
	ON_EN_CHANGE(IDC_EDIT_USER, &CDlgNewLogin::OnEnChangeEditUser)
	ON_WM_NCLBUTTONDOWN()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgNewLogin message handlers

BOOL CDlgNewLogin::OnInitDialog() 
{
	// CDialog::OnInitDialog();

	int32 iTitleHeight = 34;
	int32 iWndWidth = 800;
	int32 iWndHeight = 390 + iTitleHeight;

	CRect rcWnd(0, 0, iWndWidth, iWndHeight);

	m_rcLeft = rcWnd;
	m_rcLeft.top += iTitleHeight;
	//m_rcLeft.right = 458;

	m_rcCaption = rcWnd;
	m_rcCaption.top = rcWnd.top ;
	m_rcCaption.bottom = iTitleHeight;
	
	//m_rcRight = rcWnd;
	//m_rcRight.top += iTitleHeight;
	//m_rcRight.left = m_rcLeft.right ;
	MoveWindow(rcWnd);

	// 宣传界面
	CGGTongApp *pApp  =  (CGGTongApp*) AfxGetApp();	
	bool32 bShowThirdLogin = pApp->m_pConfigInfo->m_bShowThirdLogin;	// 是否显示三方登录
	if (!pApp->m_pConfigInfo->m_bMember)
	{
		bShowThirdLogin = false;
	}
	m_bShowWeb = false;
	int32 iControlYPos = 54;	// 控件开始位置
	int32 iYSpace = 16;			// 控件y方向间距
	
	if (!bShowThirdLogin)
	{
		iControlYPos = 83;		
	}

	GetDlgItem(IDC_BTN_REG)->ShowWindow(SW_HIDE);	// 免费注册  IDC_BTN_PASSWORD_RETAKE

	m_pImgLoginClose = Image::FromFile(CPathFactory::GetImageLoginClosePath());
	int iHeight = (iTitleHeight - m_pImgLoginClose->GetHeight()/3)/2;
	CRect rcTmp(rcWnd.right-m_pImgLoginClose->GetWidth()-5, iHeight, rcWnd.right-5, iHeight + m_pImgLoginClose->GetHeight()/3);
	AddButton(rcTmp, m_pImgLoginClose, 3, ID_LOGIN_CLOSE);	//关闭按钮

	m_pImgLoginSetting = Image::FromFile(CPathFactory::GetImageLoginSettingPath());
	rcTmp.right = rcTmp.left - 5;
	rcTmp.left  = rcTmp.right - m_pImgLoginSetting->GetWidth();
	AddButton(rcTmp, m_pImgLoginSetting, 3, ID_LOGIN_SETTING);	//设置按钮

	rcTmp.left = KRightStartPosition +KMidSpace;
	rcTmp.top =  iTitleHeight+ iControlYPos;
	rcTmp.right = rcTmp.left + 278;
	rcTmp.bottom = rcTmp.top + 36;
	GetDlgItem(IDC_EDIT_USER)->MoveWindow(rcTmp);  // 用户名

	CRect rcHide = rcTmp;
	rcHide.right = rcHide.left;
	rcHide.bottom = rcHide.top;
	GetDlgItem(IDC_EDIT_HIDE)->MoveWindow(rcHide);  // 获取对话框焦点的控件

	rcTmp.left = KRightStartPosition +KMidSpace;
	rcTmp.top = rcTmp.bottom + iYSpace;
	rcTmp.right = rcTmp.left + 278;
	rcTmp.bottom = rcTmp.top + 36;
	GetDlgItem(IDC_EDIT_PWD)->MoveWindow(rcTmp);  // 密码

 	m_pImgCheck = Image::FromFile(CPathFactory::GetImageLoginCheckPath());
 	rcTmp.right = rcTmp.left + m_pImgCheck->GetWidth();
 	rcTmp.top = rcTmp.bottom + iYSpace;
 	rcTmp.bottom = rcTmp.top + m_pImgCheck->GetHeight()/3;
	AddButton(rcTmp, m_pImgCheck, 3, ID_LOGIN_CHECK_PWD);	// 记住密码		
	if (!pApp->m_pConfigInfo->m_bMember)
	{
		m_mapBtn[ID_LOGIN_CHECK_PWD].EnableButton(FALSE);
	} 		

	CRect rcSavePwd = rcTmp;
	rcSavePwd.top += 1;
	rcSavePwd.left = rcTmp.right + 5;
	rcSavePwd.right = rcSavePwd.left + 70;
	GetDlgItem(IDC_STATIC_SAVE_PWD)->MoveWindow(rcSavePwd);	
		
	CRect rcAutoLogin = rcTmp;
	rcAutoLogin.left  =  KRightStartPosition + 173;	
	rcAutoLogin.right = rcAutoLogin.left + 70;
	GetDlgItem(IDC_STATIC_AUTO_LOGIN)->MoveWindow(rcAutoLogin);		//自动登录

	CRect rcAutoLoginCheck = rcAutoLogin;
	rcAutoLoginCheck.left = rcAutoLogin.left - m_pImgCheck->GetWidth()  - 5 ;
	rcAutoLoginCheck.right = rcAutoLogin.left - 5;
	AddButton(rcAutoLoginCheck, m_pImgCheck, 3, ID_LOGIN_CHECK_AUTOLOGIN);	// 自动登录check

	CRect rcRetake = rcAutoLogin;
	rcRetake.left = rcRetake.right + 6 ;
	rcRetake.right = rcRetake.left + 70;			
	GetDlgItem(IDC_BTN_PASSWORD_RETAKE)->MoveWindow(rcRetake);	//忘记密码  

	//CRect rcRetake = rcTmp;
	//rcRetake.top += 1;
	//rcRetake.left += 80 ;
	//rcRetake.right = rcRetake.left + 120;			
	//GetDlgItem(IDC_BTN_PASSWORD_RETAKE)->MoveWindow(rcRetake);	//忘记密码  
	
	m_pImgLoginConnect = Image::FromFile(CPathFactory::GetImageLoginConnectPath());
	rcTmp.left = KRightStartPosition + KMidSpace;
	rcTmp.top = rcTmp.bottom + iYSpace ;
	rcTmp.right = rcTmp.left + 176;// pImage->GetWidth();
	rcTmp.bottom = rcTmp.top + m_pImgLoginConnect->GetHeight()/3;
	//	离线登录情况下的登录按钮
	ASSERT(pApp);
	ASSERT(pApp->m_pDocument);
	ASSERT(pApp->m_pDocument->m_pAbsCenterManager);
	if (pApp->m_pDocument->m_pAbsCenterManager->IsOffLineLogin())
	{
		AddButton(rcTmp, m_pImgLoginConnect, 3, ID_LOGIN_CONNECT, L"脱机登录");	// 登录
	}
	else
	{
		AddButton(rcTmp, m_pImgLoginConnect, 3, ID_LOGIN_CONNECT, L"登 录");	// 登录

	}
	
	CRect rcOutline = rcTmp;
	rcOutline.top = rcTmp.bottom +iYSpace;	
	rcOutline.right = rcOutline.left + 50;
	rcOutline.bottom = rcOutline.top + 10;
	GetDlgItem(IDC_BTN_VISITOR_LOGIN)->MoveWindow(rcOutline);			// 游客登录

	//	离线登录情况下的游客登录
	if (pApp->m_pDocument->m_pAbsCenterManager->IsOffLineLogin())
	{
		GetDlgItem(IDC_BTN_VISITOR_LOGIN)->ShowWindow(SW_HIDE);
	}
	else
	{
		GetDlgItem(IDC_BTN_VISITOR_LOGIN)->ShowWindow(SW_SHOW);

	}

	if (GetDlgItem(IDC_BUTTON_OFFLINE))
	{
		GetDlgItem(IDC_BUTTON_OFFLINE)->ShowWindow(SW_HIDE);
	}
	m_pImgRegister = Image::FromFile(_T("./image/Registered_btn.png"));
	rcTmp.left = rcTmp.right + 4;
	rcTmp.right = rcTmp.left + 100;// pImage->GetWidth();
	rcTmp.bottom = rcTmp.top + m_pImgRegister->GetHeight()/3;
	AddButton(rcTmp, m_pImgRegister, 3, ID_LOGIN_REGISTERED, L"注册享特权");	// 注册

	

	if (bShowThirdLogin)
	{
		m_pImgLoginQQ = Image::FromFile(L"./image/Login_QQ.png");
		rcTmp.left   = KRightStartPosition + 128;
		rcTmp.top    = rcTmp.bottom + 87+iYSpace;
		rcTmp.right  = rcTmp.left + m_pImgLoginQQ->GetWidth();
		rcTmp.bottom = rcTmp.top + m_pImgLoginQQ->GetHeight()/3; 
		AddButton(rcTmp, m_pImgLoginQQ, 3, ID_LOGIN_THIRDPARTY_QQ, L""); //第三方QQ登录	

		m_pImgLoginWechat = Image::FromFile(L"./image/Login_Wechat.png");
		rcTmp.left   = rcTmp.right + 18;
		rcTmp.right  = rcTmp.left + m_pImgLoginWechat->GetWidth();
		rcTmp.bottom = rcTmp.top + m_pImgLoginWechat->GetHeight()/3; 
		AddButton(rcTmp, m_pImgLoginWechat, 3,ID_LOGIN_THIRDPARTY_WECHAT, L""); //第三方微信登录
	}
	
	//
	rcTmp.left = KRightStartPosition;
	rcTmp.top = iWndHeight - 77 ;
	rcTmp.right = rcWnd.right;
	rcTmp.bottom = iWndHeight - 57;
	GetDlgItem(IDC_STATIC_PROMPT)->MoveWindow(rcTmp); // 提示1

	rcTmp.top =  iWndHeight - 77 ;
	rcTmp.bottom =iWndHeight - 57;
	GetDlgItem(IDC_STATIC_PROMPT2)->MoveWindow(rcTmp); // 提示2

	///////////////////////////////////////////////////

	COLORREF activeColor = RGB(255,56,55);
	COLORREF normalColor = RGB(177,177,177);

	m_CtrlEditName.SetBitmaps(IDB_BITMAP_BUSER);
	m_CtrlEditName.SetOwner(this);
	m_CtrlEditName.SetButtonClickedMessageId(WM_USER_EDITWITHBUTTON_CLICKED);
	m_CtrlEditName.SetButtonExistsAlways(FALSE);
	m_CtrlEditName.SetTipTextColor(RGB(122,125,128));
	m_CtrlEditName.SetActiveFrameColor(activeColor);
	m_CtrlEditName.SetNormalFrameColor(normalColor);
	m_CtrlEditName.SetTipText(L"账号");
	
	m_CtrlEditPassword.SetBitmaps(IDB_BITMAP_BPASSWORD);
	m_CtrlEditPassword.SetOwner(this);
	m_CtrlEditPassword.SetButtonClickedMessageId(WM_USER_EDITWITHBUTTON_CLICKED);
	m_CtrlEditPassword.SetButtonExistsAlways(FALSE);
	m_CtrlEditPassword.SetTipTextColor(RGB(122,125,128));
	m_CtrlEditPassword.SetActiveFrameColor(activeColor);
	m_CtrlEditPassword.SetNormalFrameColor(normalColor);
	m_CtrlEditPassword.SetTipText(L"密码");
	m_CtrlEditPassword.EnableTrackingToolTips();

	LOGFONT logFont;
    //内容字体设置
    logFont.lfHeight =17;
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
	_tcscpy(logFont.lfFaceName, gFontFactory.GetExistFontName((LPTSTR)KFontFamily));	//...
	
	CRect rect;
	GetDlgItem(IDC_EDIT_USER)->GetWindowRect(&rect);
	
	//
	m_CtrlEditName.SetTipFont(&logFont);
	m_CtrlEditPassword.SetTipFont(&logFont);

	//
	logFont.lfHeight = 21;
	m_CtrlEditName.SetEditFont(&logFont);
	m_CtrlEditPassword.SetEditFont(&logFont);

	//
	if ( rect.Width() > 0)
		m_CtrlEditName.SetEditArea(5, 5, 10, 0, rect.Width(), 36);
	else
		m_CtrlEditName.SetEditArea(5, 5, 10, 0, 200, 36);
	
	//忽略精度问题
	//lint --e(569)
	m_CtrlEditPassword.SetPasswordCharEx(_T('●'));
	if ( rect.Width() > 0)
		m_CtrlEditPassword.SetEditArea(5, 5, 10, 0, rect.Width(), 36);
	else
		m_CtrlEditPassword.SetEditArea(5, 5, 10, 0, 200, 36);
	
	// 字体调整
	if ( m_fontStaticText.m_hObject == 0 )
	{
		LOGFONT lf = {0};
		lf.lfHeight = KGdiFontSize;
		_tcscpy(lf.lfFaceName, gFontFactory.GetExistFontName((LPTSTR)KFontFamily));	//...
		m_fontStaticText.CreateFontIndirect(&lf);
		ASSERT( m_fontStaticText.m_hObject );
		
		lf.lfHeight = 18;
		_tcscpy(lf.lfFaceName, gFontFactory.GetExistFontName(L"微软雅黑"));	//...
		m_fontCheckText.CreateFontIndirect(&lf);
		ASSERT( m_fontCheckText.m_hObject );
	}

	///////////////////////////////////////////////////////////
	CString StrIconPath = CPathFactory::GetImageMainIcon32Path();
	HICON hIcon = (HICON)LoadImage(AfxGetInstanceHandle(), StrIconPath, IMAGE_ICON, 0, 0, LR_DEFAULTSIZE|LR_LOADFROMFILE);

	if ( NULL == hIcon )
	{
		hIcon = LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME));
		SetIcon(hIcon,TRUE);
		SetIcon(hIcon,FALSE);
	}
	else
	{
		SetIcon(hIcon,TRUE);
		SetIcon(hIcon,FALSE);
	}	
	SetClassLong(m_hWnd, GCL_HICON, (LONG)hIcon);
	CenterWindow();

// #ifdef DEBUG
// 	SetWindowText(AfxGetApp()->m_pszAppName + CString(_T(" - 用户认证")));
// #endif
// 	{
// 		ModifyStyleEx(0, WS_EX_APPWINDOW, 0);
// 	}

	SetWindowText(AfxGetApp()->m_pszAppName);
	m_CtrlProgressLogin.SetShowText(TRUE);
	
	// 默认提供的服务器:
	m_HostInfoDefault.StrHostName    = L"DefaultHost";
	m_HostInfoDefault.StrHostAddress = L"192.168.0.189";
	m_HostInfoDefault.uiHostPort	 = 32111;

	// 从 serverinfo.xml 中读服务器数据:
	bool32 bValidServerQuote = false;
	bool32 bValidServerNews  = false;

	GetServerInfoFromXml(bValidServerQuote, bValidServerNews); 
	
	if( !bValidServerQuote )
	{
		// 行情服务器数据读取失败,使用默认配置
		////ASSERT(0);		
		m_aHostInfo.Add(m_HostInfoDefault);
	}

	if ( !bValidServerNews )
	{
		// 资讯服务器数据读取失败,使用默认配置
		////ASSERT(0);
		// ...fangz1112	
	}

	// 从UserInfo.xml 中读取保存的用户信息
	if ( !GetUserInfoFromXml() )
	{
		////ASSERT(0);
		m_iHostIndex	= 0;
		m_bSavePwd		= false;
		m_StrUserName	= L"";
		m_StrPassword	= L"";
		m_bAutoLogin	= FALSE;
		m_bShowSCToolBar = FALSE;
	}

	/////////////////////////////////////////////////////
	//===================CODE CLEAN======================
	//////////////////////////////////////////////////////
	//// 获取智能选股的IP与选股模型
	//GetPickModelInfoFromXml(); 

	UpdateData(FALSE);

	//// 通知初始化对话框成功， 在这里创建m_pAbsCenterManager等对象
	//if (!m_LoginDlgObserver.OnLoginDlgDoModalSuccess())
	//{
	//	// 只能退出了
	//	MessageBox(_T("登录初始化错误，无法继续"), _T("抱歉"), MB_ICONERROR);
	//	OnCancel();
	//	return FALSE;
	//}

	// 根据当前状态设置按钮状态
	CGGTongDoc *pDoc = pApp->m_pDocument;
	CAbsCenterManager *pAbsCenterManager = pDoc->m_pAbsCenterManager;
	ASSERT(NULL != pDoc->m_pAutherManager);
	SetControlsState(pAbsCenterManager->GetLoginState());
	SetPromptInfo2(L"");

	pAbsCenterManager->SetLoginHWnd(GetSafeHwnd());
//	pAbsCenterManager->SetOwnerWnd(GetSafeHwnd());

	// 初始化各控件
	if ( m_aUserInfo.GetSize() == 0 )
	{
		//没有用户
		T_UsersInfo stUserInfo;
		m_aUserInfo.Add(stUserInfo);
	}

	// 默认选择的服务器
	m_indexServerSel = 0;

	// 用户信息
	int32 iUserIndex = 0;
	for ( ; iUserIndex < m_aUserInfo.GetSize() ; iUserIndex++)
	{
		if ( m_aUserInfo[iUserIndex].BeLastUser )
		{
			m_StrUserName = m_aUserInfo[iUserIndex].StrUserName;
			m_StrPassword = m_aUserInfo[iUserIndex].StrPassword;
			m_indexServerSel = m_aUserInfo[iUserIndex].IndexServer;
			m_bSavePwd = m_aUserInfo[iUserIndex].BeSavePassword;
			m_bAutoLogin  = m_aUserInfo[iUserIndex].IsAtutoLogin;
			m_bShowSCToolBar = m_aUserInfo[iUserIndex].IsShowSCToolBar;
			UpdateData(false);
			break;
		}
	}

	{		
		_MYTRACE(L"DlgLogin::m_hWnd: %d", GetSafeHwnd());				
	
		CString StrHwnd;
		StrHwnd.Format(L"%d", (long)GetSafeHwnd());
		::WritePrivateProfileString(L"TRACE2", L"HWND",	StrHwnd, L"./trace.ini");
	}
	
	//获取代理服务
	T_ProxyInfo  proxyInfo;
	GetProxyInfoFromXml(proxyInfo);

	CString strImgCaption = CPathFactory::GetImageLoginCaptionPath();
	m_pImgCaption = Image::FromFile(CPathFactory::GetImageLoginCaptionPath());
	m_pImgLogo = Image::FromFile(L"image//loginLogo.png"/*CPathFactory::GetImageLoginLogoPath()*/);
	CString strTop = CPathFactory::GetImageLoginTopPath();
	m_pImgTop = Image::FromFile(CPathFactory::GetImageLoginTopPath());
	if (!bShowThirdLogin)
	{
		TCHAR szAbsPathFile[MAX_PATH+1]={NULL};
		GetModuleFileName(NULL, szAbsPathFile, MAX_PATH);
		PathRemoveFileSpec(szAbsPathFile);
		CString strPath = szAbsPathFile;
		strPath += L"\\";
		m_pImgBottom = Image::FromFile(strPath + L"image//no_thrd_login_bg.png");		
	}
	else
	{
		m_pImgBottom = Image::FromFile(CPathFactory::GetImageLoginBottomPath());
	}

	m_pImgUnCheck = Image::FromFile(CPathFactory::GetImageLoginUnCheckPath());
	if (!pApp->m_pConfigInfo->m_bMember)
	{
		GetDlgItem(IDC_BTN_REG)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_BTN_PASSWORD_RETAKE)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_BTN_VISITOR_LOGIN)->ShowWindow(SW_HIDE);		
		GetDlgItem(IDC_EDIT_USER)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_PWD)->EnableWindow(FALSE);
		if ((L"" != pApp->m_pConfigInfo->m_StrDefUser) && (L"" != pApp->m_pConfigInfo->m_StrDefPwd))
		{
			m_StrUserName = pApp->m_pConfigInfo->m_StrDefUser;
			m_StrPassword = pApp->m_pConfigInfo->m_StrDefPwd;
			m_bSavePwd = 1;
			//m_bAutoLogin  = 1;
		}
	}
	else
	{
		if (!pApp->m_pConfigInfo->m_bRegOn)
		{
			GetDlgItem(IDC_BTN_REG)->ShowWindow(SW_HIDE);
		}

		if(!pApp->m_pConfigInfo->m_bZHMMOn)
		{
			GetDlgItem(IDC_BTN_PASSWORD_RETAKE)->ShowWindow(SW_HIDE);
		}
	}

	if( m_bAutoLogin )
	{
		m_bSavePwd = TRUE;
		m_mapBtn[ID_LOGIN_CHECK_AUTOLOGIN].SetImage(m_pImgCheck);

		if( m_nAutoLoginTimer <= 0 )
		{
			m_nAutoLoginTimer = SetTimer(111,100,NULL);
		}
	}
	else
	{
		m_mapBtn[ID_LOGIN_CHECK_AUTOLOGIN].SetImage(m_pImgUnCheck);
	}

	if (m_bSavePwd)
	{
		m_mapBtn[ID_LOGIN_CHECK_PWD].SetImage(m_pImgCheck);
	}
	else
	{
		m_mapBtn[ID_LOGIN_CHECK_PWD].SetImage(m_pImgUnCheck);
	}

	HCURSOR cursor = ::LoadCursor(NULL, MAKEINTRESOURCE(32649));
	m_CtrBtnReg.SetCursor(cursor);
    m_CtrBtnReg.SetHoverTextColor(RGB(198,177,31));
    m_CtrBtnReg.SetTextColor(RGB(62,69,81)/*RGB(116,89,28)*/);
    //m_CtrBtnReg.SetButtonFont(_T("宋体"), 15);
	
	m_CtrBtnVisitor.SetCursor(cursor);
    m_CtrBtnVisitor.SetHoverTextColor(RGB(198,177,31));
    m_CtrBtnVisitor.SetTextColor(FONT_COLOR);

	m_CtrBtnOffline.SetCursor(cursor);
	m_CtrBtnOffline.SetHoverTextColor(RGB(198,177,31));
	m_CtrBtnOffline.SetTextColor(FONT_COLOR);

	m_CtrBtnPasswordRetake.SetCursor(cursor);
	m_CtrBtnPasswordRetake.SetHoverTextColor(RGB(198,177,31));
	m_CtrBtnPasswordRetake.SetTextColor(FONT_COLOR);






	if (pApp->m_pConfigInfo->m_StrDefUser == _T("@"))
	{
		m_CtrBtnVisitor.ShowWindow(SW_HIDE);
	}


    //m_CtrBtnVisitor.SetButtonFont(_T("宋体"), 15);


    //m_CtrBtnPasswordRetake.SetButtonFont(_T("宋体"), 15);

	m_strAppName = pApp->m_pszAppName;
	if (pApp->m_pConfigInfo->m_StrBtName.GetLength() > 0)
	{
		GetDlgItem(IDC_BTN_REG)->SetWindowText(pApp->m_pConfigInfo->m_StrBtName);
	}

	if (pApp->m_pConfigInfo->m_StrZHMMBtName.GetLength() > 0)
	{
		GetDlgItem(IDC_BTN_PASSWORD_RETAKE)->SetWindowText(pApp->m_pConfigInfo->m_StrZHMMBtName);
	}

	m_CtrlEditPassword.SetLimitText(KPasswdMaxLength);
	m_CtrlEditPassword.SetActiveFrameColor(activeColor);
	m_CtrlEditName.SetLimitText(KAccountMaxLength);
	m_CtrlEditName.SetActiveFrameColor(activeColor);
	if(NULL == m_pDlgComment)
	{
		m_pDlgComment = new CDlgNewCommentIE(this);
		m_pDlgComment->Create(IDD_DIALOG_NEW_COMMENT, this);
	}

	m_CtrBtnReg.SetButtonFont(KFontFamily,KGdiFontSize);
	m_CtrBtnVisitor.SetButtonFont(KFontFamily,KGdiFontSize);
	m_CtrBtnOffline.SetButtonFont(KFontFamily,KGdiFontSize);
	m_CtrBtnPasswordRetake.SetButtonFont(KFontFamily,KGdiFontSize);
	GetDlgItem(IDC_STATIC_SAVE_PWD)->SetFont(&m_fontStaticText);
	GetDlgItem(IDC_STATIC_AUTO_LOGIN)->SetFont(&m_fontStaticText);

	
	UpdateData(FALSE);
	//SetWindowPos(&CWnd::wndTopMost,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
	return TRUE; 
}

void CDlgNewLogin::DrawLogo(Gdiplus::Graphics &graphics, const RectF &rcF)
{
	if(NULL != m_pImgLogo)
	{
		if (rcF.Height < m_pImgLogo->GetHeight() || rcF.Width < (m_pImgLogo->GetWidth()))
		{
			return;
		}

		RectF destRect;
		destRect.X = rcF.X ;
		destRect.Y = rcF.Y + (rcF.Height - m_pImgLogo->GetHeight())/2 - 2;
		destRect.Height = m_pImgLogo->GetHeight();
		destRect.Width = m_pImgLogo->GetWidth();
		graphics.DrawImage(m_pImgLogo, destRect);
	}
}

void CDlgNewLogin::DrawTitleBar()
{
	CWindowDC dc(this);
	CDC memDC;
	memDC.CreateCompatibleDC(&dc);
	CBitmap bmp;
	CGGTongApp* pApp = (CGGTongApp*)AfxGetApp();
	CString strLoginTitleText = pApp->m_pConfigInfo->m_StrLoginTitleText;
	
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

	DrawLogo(graphics, RectF(fRect.X + KMidSpaceLogo, fRect.Y, fRect.Width, fRect.Height));


	// 标题栏文本
	{
		RectF grect;
		grect.X = (REAL)(m_rcLeft.left + 30 );
		grect.Y = (REAL)(m_rcCaption.top);
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
		m_Font.m_StrName = gFontFactory.GetExistFontName((LPTSTR)KFontFamily);	//...
		m_Font.m_Size	 = KGdiPlusFontSize;
		m_Font.m_iStyle	 = FontStyleBold;	
		Gdiplus::FontFamily fontFamily(m_Font.m_StrName);
		Gdiplus::Font font(&fontFamily, m_Font.m_Size, m_Font.m_iStyle, UnitPoint);


		RectF rcBound;
		PointF point;
		graphics.MeasureString(strLoginTitleText, strLoginTitleText.GetLength(), &font, point, &strFormat, &rcBound);
		grect.Width = rcBound.Width;
		
		SolidBrush brush((ARGB)Color::White);
		brush.SetColor(Color::Color(255, 255, 255/*185,185,185*/));
		graphics.DrawString(strLoginTitleText, strLoginTitleText.GetLength(), &font, grect, &strFormat, &brush);
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

	dc.BitBlt(m_rcCaption.left, m_rcCaption.top, m_rcCaption.Width(), m_rcCaption.Height(), &memDC,m_rcCaption.left, m_rcCaption.top, SRCCOPY);
 	dc.SelectClipRgn(NULL);
 	memDC.DeleteDC();
 	bmp.DeleteObject();
	InvalidateRect(m_rcCaption);
}

BOOL CDlgNewLogin::OnEraseBkgnd(CDC* pDC)
{
	CDC memDC;
	memDC.CreateCompatibleDC(pDC);
	CBitmap bmp;
	CRect rcWindow;
	CGGTongApp* pApp = (CGGTongApp*)AfxGetApp();
	CString strLoginTitleText = pApp->m_pConfigInfo->m_StrLoginTitleText;
	GetWindowRect(&rcWindow);
	rcWindow.OffsetRect(-rcWindow.left, -rcWindow.top);
	
	bmp.CreateCompatibleBitmap(pDC, rcWindow.Width(), rcWindow.Height());
	memDC.SelectObject(&bmp);
	memDC.FillSolidRect(0, 0, rcWindow.Width(), rcWindow.Width(), RGB(255,255,255));
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

	DrawLogo(graphics, RectF(fRect.X + KMidSpaceLogo, fRect.Y, fRect.Width, fRect.Height));

	// 标题栏文本
	{
		RectF grect;
		grect.X = (REAL)(m_rcLeft.left + 30 );
		grect.Y = (REAL)(m_rcCaption.top);
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
		m_Font.m_StrName = gFontFactory.GetExistFontName((LPTSTR)KFontFamily);	//...
		m_Font.m_Size	 = KGdiPlusFontSize;
		m_Font.m_iStyle	 = FontStyleBold;	
		Gdiplus::FontFamily fontFamily(m_Font.m_StrName);
		Gdiplus::Font font(&fontFamily, m_Font.m_Size, m_Font.m_iStyle, UnitPoint);


		RectF rcBound;
		PointF point;
		graphics.MeasureString(strLoginTitleText, strLoginTitleText.GetLength(), &font, point, &strFormat, &rcBound);
		grect.Width = rcBound.Width;

		SolidBrush brush((ARGB)Color::White);
		brush.SetColor(Color::Color(255, 255, 255/*185,185,185*/));
		graphics.DrawString(strLoginTitleText, strLoginTitleText.GetLength(), &font, grect, &strFormat, &brush);
	}

	// left
	m_pImgTop;
	fRect.X = (REAL)m_rcLeft.left;
	fRect.Y = (REAL)m_rcLeft.top;
	fRect.Width = (REAL)m_rcLeft.Width();
	fRect.Height = (REAL)m_rcLeft.Height();
	if (NULL != m_pImgTop && !m_bShowWeb)
	{
		graphics.DrawImage(m_pImgTop, fRect, 0, 0, (REAL)m_pImgTop->GetWidth(), (REAL)m_pImgTop->GetHeight(), UnitPixel);
	}

	// right
	//fRect.X = (REAL)m_rcRight.left;
	//fRect.Y = (REAL)m_rcRight.top;
	//fRect.Width = (REAL)m_rcRight.Width();
	//fRect.Height = (REAL)m_rcRight.Height();
	//if (NULL != m_pImgBottom)
	//{
	//	graphics.DrawImage(m_pImgBottom, fRect, 0, 0, (REAL)m_pImgBottom->GetWidth(), (REAL)m_pImgBottom->GetHeight(), UnitPixel);
	//}
	

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

void CDlgNewLogin::OnMouseMove(UINT nFlags, CPoint point)
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

LRESULT CDlgNewLogin::OnMouseLeave(WPARAM wParam, LPARAM lParam)       
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

void CDlgNewLogin::OnLButtonDown(UINT nFlags, CPoint point) 
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

void CDlgNewLogin::OnLButtonUp(UINT nFlags, CPoint point) 
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

BOOL CDlgNewLogin::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	// 取消登录
	CGGTongApp *pApp  =  (CGGTongApp*) AfxGetApp();	
	if (NULL == pApp)
	{
		ASSERT(0);
	}

	CGGTongDoc *pDoc = pApp->m_pDocument;
	CAbsCenterManager *pAbsCenterManager = pDoc->m_pAbsCenterManager;
	if (!pAbsCenterManager)
	{
		return FALSE;
	}
	
	int32 iID = (int32)wParam;
	std::map<int, CNCButton>::iterator it = m_mapBtn.find(iID);
	if (m_mapBtn.end() != it)
	{
		switch (iID)
		{
		case ID_LOGIN_SETTING:
			{	
				// 设置
				SetNet();
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
				Connect(FALSE);
				
				break;
			}
		case ID_LOGIN_REGISTERED:
			{	
				OnButtonReg();
				break;
			}
		case ID_LOGIN_CANCEL:
			{	
				
				
				if (ELSAuthing != pAbsCenterManager->GetLoginState())
				{
					////ASSERT(0);
					// 			m_bCancelAuth = true;
					// 			
					// 			pDoc->m_pAutherManager->GetInterface()->Stop();
					// 			pAbsCenterManager->m_eLoginState = NEW_ELSNotLogin;
					// 			SetControlsState(pAbsCenterManager->m_eLoginState);
					// 			ReSetLogData();
					// 			
					// 			return;
				}
				m_bCancelAuth = true;
				
				pAbsCenterManager->SetLoginState(ELSNotLogin);
				SetControlsState(pAbsCenterManager->GetLoginState());
				ReSetLogData();
				break;
			}
		case ID_LOGIN_CHECK_AUTOLOGIN:
			{	
				m_bAutoLogin = !m_bAutoLogin;
				// 自动登录
				if( m_bAutoLogin )
				{
					if( !m_bSavePwd )
					{
						m_bSavePwd = TRUE;
						if (m_mapBtn[ID_LOGIN_CHECK_PWD].IsEnable())
						{
							m_mapBtn[ID_LOGIN_CHECK_PWD].SetImage(m_pImgCheck);
						}						
					}
					m_mapBtn[iID].SetImage(m_pImgCheck);
				}
				else
				{
					m_mapBtn[iID].SetImage(m_pImgUnCheck);
				}

				break;
			}
		case ID_LOGIN_CHECK_PWD:
			{	
				// 记住密码
				m_bSavePwd = !m_bSavePwd;
				if (m_bSavePwd)
				{
					m_mapBtn[iID].SetImage(m_pImgCheck);
				}
				else
				{
					m_mapBtn[iID].SetImage(m_pImgUnCheck);
					if( m_bAutoLogin )
					{
						m_bAutoLogin = FALSE;
						m_mapBtn[ID_LOGIN_CHECK_AUTOLOGIN].SetImage(m_pImgUnCheck);
					}
				}
				break;
			}
		case ID_LOGIN_THIRDPARTY_QQ:
			{
				ShowThirdPartyLogin(iID);
				return CDialog::OnCommand(wParam, lParam);
			}
		case ID_LOGIN_THIRDPARTY_WECHAT:
			{
// 				CGGTongApp *pApp  =  (CGGTongApp*) AfxGetApp();	
// 				if ( !pApp->m_pConfigInfo->m_StrWeChatUrl.IsEmpty() )
// 				{
// 					ShellExecute(0, L"open", pApp->m_pConfigInfo->m_StrWeChatUrl, NULL, NULL, SW_NORMAL);
// 				}
				ShowThirdPartyLogin(iID);
				return CDialog::OnCommand(wParam, lParam);
			}
		default:
			{
				break;
			}
		}
	}

	return CDialog::OnCommand(wParam, lParam);
}

void CDlgNewLogin::OnPaint() 
{
	CPaintDC dc(this);
}

void CDlgNewLogin::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	OnLButtonDown(nFlags, point);
	CDialog::OnLButtonDblClk(nFlags, point);
}

HBRUSH CDlgNewLogin::OnCtlColor( CDC* pDC, CWnd* pWnd, UINT nCtlColor )
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	
	if (nCtlColor == CTLCOLOR_STATIC)
	{	
		CString s;
		pWnd->GetWindowText(s);
		
		pDC->SetBkMode(TRANSPARENT);
		pDC->SetTextColor(FONT_COLOR);
		//pDC->SelectObject(&m_fontStaticText);

		int32 iID = pWnd->GetDlgCtrlID();
		if (IDC_EDIT_USER!=iID && IDC_EDIT_PWD!=iID)
		{
			return (HBRUSH)GetStockObject(NULL_BRUSH);
		}
	}
	else if ( CTLCOLOR_BTN == nCtlColor )
	{
		if ( IDC_CHECK_SAVEPASSWORD == pWnd->GetDlgCtrlID() || IDC_CHECK_OPTIMIZE_SERVER == pWnd->GetDlgCtrlID() )
		{
			//return (HBRUSH)GetStockObject(NULL_BRUSH);
		}
	}

	return hbr;
}

void CDlgNewLogin::SetControlsState(E_LoginState eLoginState)
{
	if (ELSNotLogin == eLoginState ||
		ELSAuthFail == eLoginState ||
		ELSLoginFail == eLoginState)
	{
		m_CtrlEditPassword.EnableWindow(TRUE);
		m_CtrlEditName.EnableWindow(TRUE);
		GetDlgItem(IDC_BTN_REG)->EnableWindow(TRUE);
		GetDlgItem(IDC_BTN_VISITOR_LOGIN)->EnableWindow(TRUE);
        GetDlgItem(IDC_BTN_PASSWORD_RETAKE)->EnableWindow(TRUE);

		map<int, CNCButton>::iterator iter;
		for (iter=m_mapBtn.begin(); iter!=m_mapBtn.end(); ++iter)
		{
			CNCButton &btnControl = iter->second;
			int32 ID = btnControl.GetControlId();

			if (ID_LOGIN_CANCEL == ID)
			{
				btnControl.SetVisiable(FALSE);
				btnControl.EnableButton(FALSE, TRUE);
			}
			else
			{
				btnControl.SetVisiable(TRUE);	
				btnControl.EnableButton(TRUE);	
			}
		}
		
		if ( ELSNotLogin == eLoginState )
		{
		//	m_CtrlStaticPrompt.SetWindowText(L"未登录");	
			m_CtrlStaticPrompt.SetWindowText(L"");
		}

		m_CtrlProgressLogin.SetRange32(0, 0);
	}
	else if (ELSAuthing == eLoginState ||
		ELSAuthSuccess == eLoginState)
	{
		m_CtrlEditPassword.EnableWindow(FALSE);
		m_CtrlEditName.EnableWindow(FALSE);
		GetDlgItem(IDC_BTN_REG)->EnableWindow(FALSE);
		GetDlgItem(IDC_BTN_VISITOR_LOGIN)->EnableWindow(FALSE);
        GetDlgItem(IDC_BTN_PASSWORD_RETAKE)->EnableWindow(FALSE);

		map<int, CNCButton>::iterator iter;
		for (iter=m_mapBtn.begin(); iter!=m_mapBtn.end(); ++iter)
		{
			CNCButton &btnControl = iter->second;
			int32 ID = btnControl.GetControlId();
			
			if (ID_LOGIN_CANCEL == ID)
			{
				btnControl.SetVisiable(TRUE);
				btnControl.EnableButton(TRUE, TRUE);		
			}

			if (ID_LOGIN_CONNECT == ID)
			{
			//	btnControl.SetVisiable(FALSE);
				btnControl.EnableButton(FALSE, TRUE);				
			}

			if (ID_LOGIN_CLOSE != ID && ID_LOGIN_CANCEL != ID && ID_LOGIN_CONNECT != ID)
			{
				btnControl.EnableButton(FALSE);
			}
		}
	}
	else if (ELSLogining == eLoginState)
	{
		m_CtrlEditPassword.EnableWindow(FALSE);
		m_CtrlEditName.EnableWindow(FALSE);
		GetDlgItem(IDC_BTN_REG)->EnableWindow(FALSE);
		GetDlgItem(IDC_BTN_VISITOR_LOGIN)->EnableWindow(FALSE);
        GetDlgItem(IDC_BTN_PASSWORD_RETAKE)->EnableWindow(FALSE);
		
		map<int, CNCButton>::iterator iter;
		for (iter=m_mapBtn.begin(); iter!=m_mapBtn.end(); ++iter)
		{
			CNCButton &btnControl = iter->second;
			int32 ID = btnControl.GetControlId();

			if (ID_LOGIN_CANCEL == ID)
			{
				btnControl.EnableButton(TRUE, TRUE);
				continue;
			}

			if (ID_LOGIN_CLOSE != ID && ID_LOGIN_CANCEL != ID)
			{
				btnControl.EnableButton(FALSE);
			}
		}	
	}
}

T_HostInfo &CDlgNewLogin::GetHostInfoDefault()
{
	return m_HostInfoDefault;
}

void CDlgNewLogin::GetServerInfoFromXml( bool32& bValidServerQuotes, bool32& bValidServerNews )
{
	// 从公共的config 目录下 Xml 文件中获得服务器信息:
	 
	bValidServerQuotes	= false;
	bValidServerNews	= false;
	
	// 1: 清空本地数据:
	m_aHostInfo.RemoveAll();

	// 2: 读文件:
	CString StrPath  = CPathFactory::GetServerInfoPath();
	
	if ( StrPath.IsEmpty() )
	{
		return;
	}

	string sPath = _Unicode2MultiChar(StrPath);
	const char* StrFilePath = sPath.c_str();
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
		
		if ( m_aHostInfo.GetSize() > 0 )
		{
			bValidServerQuotes = true;
		}
	}


	// BackupServerPort, 备用行情端口
	m_vUiServerBackup.clear();
	TiXmlElement* pBackupServerPort = pRootElement->FirstChildElement(KStrElementNameBackupServerPort);
	if( NULL != pBackupServerPort && NULL != pBackupServerPort->Value()  )
	{
		ASSERT( 0 == strcmp(pBackupServerPort->Value(), KStrElementNameBackupServerPort) );
		TiXmlElement* pServerElement = pBackupServerPort->FirstChildElement(KStrElementNameServer);

		for ( ; NULL!=pServerElement ; pServerElement=pServerElement->NextSiblingElement(KStrElementNameServer) )
		{
			T_HostInfo ServerInfo;
			// 端口:
			const char* StrPort = pServerElement->Attribute(KStrElementAttriServerPort);

			if ( NULL == StrPort )
			{
				pServerElement = pServerElement->NextSiblingElement();
				continue;
			} 

			UINT  uiHostPort		= (UINT)atoi(StrPort);
			m_vUiServerBackup.push_back(uiHostPort);
		}
	}



    // ServerInfoCenter, 资讯中心服务器
    TiXmlElement* pSInfoCenterElement = pRootElement->FirstChildElement(KStrElementAttriServerInfoCenter); 
    if ( NULL != pSInfoCenterElement && NULL != pSInfoCenterElement->Value() )
    {
        ASSERT( 0 == strcmp(pSInfoCenterElement->Value(), KStrElementAttriServerInfoCenter) );

        TiXmlElement* pServerInfoCenter = pSInfoCenterElement->FirstChildElement();

        if ( NULL != pServerInfoCenter )
        {
            const char* StrAddress = pServerInfoCenter->Attribute(KStrElementAttriServerIp);

			double dbPort = 0;
			pServerInfoCenter->Attribute(KStrElementAttriServerPort, &dbPort);

            if ( NULL != pServerInfoCenter )
            {
                TCHAR TStrServerAdd[1024];
                memset(TStrServerAdd, 0, sizeof(TStrServerAdd));
                MultiCharCoding2Unicode(EMCCUtf8, StrAddress, strlen(StrAddress), TStrServerAdd, sizeof(TStrServerAdd) / sizeof(TCHAR));

                if ( lstrlen(TStrServerAdd) > 0 )
                {
                    CGGTongDoc* pDoc = (CGGTongDoc*)AfxGetDocument();
                    if(pDoc)
                    {
                        pDoc->SetInfoCenterIP((CString)TStrServerAdd);
						pDoc->SetInfoCenterPort((int)dbPort);
                    }
                }
            }
        }
    }

	// Gateway, 量化gateway协议地址
	TiXmlElement* pGateWayElement = pRootElement->FirstChildElement(KStrElementAttriGateWayInfo); 
	if ( NULL != pGateWayElement && NULL != pGateWayElement->Value() )
	{
		ASSERT( 0 == strcmp(pGateWayElement->Value(), KStrElementAttriGateWayInfo) );

		TiXmlElement* pGateWayInfo = pGateWayElement->FirstChildElement();

		if ( NULL != pGateWayInfo )
		{
			const char* StrAddress = pGateWayInfo->Attribute(KStrElementAttriServerIp);

			double dbPort = 0;
			pGateWayInfo->Attribute(KStrElementAttriServerPort, &dbPort);

			if ( NULL != pGateWayInfo )
			{
				TCHAR TStrServerAdd[1024];
				memset(TStrServerAdd, 0, sizeof(TStrServerAdd));
				MultiCharCoding2Unicode(EMCCUtf8, StrAddress, strlen(StrAddress), TStrServerAdd, sizeof(TStrServerAdd) / sizeof(TCHAR));

				if ( lstrlen(TStrServerAdd) > 0 )
				{
					CGGTongDoc* pDoc = (CGGTongDoc*)AfxGetDocument();
					if(pDoc)
					{
						pDoc->SetGateWayIP((CString)TStrServerAdd);
						pDoc->SetGateWayPort(dbPort);
					}
				}
			}
		}
	}

	// QueryStockIP, 获取新股协议地址
	TiXmlElement* pQueryStockElement = pRootElement->FirstChildElement(KStrElementAttriQueryStockInfo); 
	if ( NULL != pQueryStockElement && NULL != pQueryStockElement->Value() )
	{
		ASSERT( 0 == strcmp(pQueryStockElement->Value(), KStrElementAttriQueryStockInfo) );

		TiXmlElement* pQueryStockInfo = pQueryStockElement->FirstChildElement();

		if ( NULL != pQueryStockInfo )
		{
			const char* StrAddress = pQueryStockInfo->Attribute(KStrElementAttriServerIp);

			double dbPort = 0;
			pQueryStockInfo->Attribute(KStrElementAttriServerPort, &dbPort);

			if ( NULL != pQueryStockInfo )
			{
				TCHAR TStrServerAdd[1024];
				memset(TStrServerAdd, 0, sizeof(TStrServerAdd));
				MultiCharCoding2Unicode(EMCCUtf8, StrAddress, strlen(StrAddress), TStrServerAdd, sizeof(TStrServerAdd) / sizeof(TCHAR));

				if ( lstrlen(TStrServerAdd) > 0 )
				{
					CGGTongDoc* pDoc = (CGGTongDoc*)AfxGetDocument();
					if(pDoc)
					{
						pDoc->SetQueryStockIP((CString)TStrServerAdd);
						pDoc->SetQueryStockPort(dbPort);
					}
				}
			}
		}
	}


	// 读取财经日历服务器地址
	TiXmlElement* pEconoServerElement = pRootElement->FirstChildElement(KStrElementnNameServerEcono); 
	if (NULL != pEconoServerElement)
	{
		TiXmlElement* pEleEcono = pEconoServerElement->FirstChildElement();
		if (NULL != pEleEcono)
		{
			const char *pAttriURL = pEleEcono->Attribute(KStrElementAttriServerEconoURL);	
			if (pAttriURL && strlen(pAttriURL) > 0)
			{
				CGGTongDoc* pDoc = (CGGTongDoc*)AfxGetDocument();
				if(pDoc)
				{
					pDoc->SetEconoServerURL(pAttriURL);
				}
			}
		}
	}
}

/////////////////////////////////////////////////////
//===================CODE CLEAN======================
//////////////////////////////////////////////////////
//void CDlgNewLogin::GetPickModelInfoFromXml()
//{
//	// 从公共的config 目录下 Xml 文件中获得服务器信息:
//	CString StrPath  = CPathFactory::GetPublicConfigPath();
//	CString strPickModelFileName = _T("StockPickingModel.xml");
//	StrPath = StrPath + strPickModelFileName;
//
//	string sPath = _Unicode2MultiChar(StrPath);
//	const char* StrFilePath = sPath.c_str();
//	TiXmlDocument myDocument = TiXmlDocument(StrFilePath);
//
//	if ( !myDocument.LoadFile())
//	{
//		return;
//	}
//
//	// XMLDATA
//	TiXmlElement* pRootElement = myDocument.RootElement();
//	if ( NULL == pRootElement )
//	{
//		return;
//	}
//
//	// PickModelServer 选股模型
//	TiXmlElement* pPickModelServersElement = pRootElement->FirstChildElement(KStrElementAttriPickModelServer);
//	if( NULL != pPickModelServersElement && NULL != pPickModelServersElement->Value()  )
//	{
//		TiXmlElement* pServerElement = pPickModelServersElement->FirstChildElement(KStrElementNameServer);
//		for( ; NULL != pServerElement; pServerElement = pServerElement->NextSiblingElement(KStrElementNameServer))
//		{
//			const char *pValue  = pServerElement->Value();
//			const char* StrAddress = pServerElement->Attribute(KStrElementAttriServerIp);
//			double dbPort = 0;
//			pServerElement->Attribute(KStrElementAttriServerPort, &dbPort);
//
//			TCHAR TStrServerAdd[1024];
//			memset(TStrServerAdd, 0, sizeof(TStrServerAdd));
//			MultiCharCoding2Unicode(EMCCUtf8, StrAddress, strlen(StrAddress), TStrServerAdd, sizeof(TStrServerAdd) / sizeof(TCHAR));
//			if ( lstrlen(TStrServerAdd) > 0 )
//			{
//				CGGTongDoc* pDoc = (CGGTongDoc*)AfxGetDocument();
//				if(pDoc)
//				{
//					pDoc->SetPickModelIP((CString)TStrServerAdd);
//					pDoc->SetPickModelPort(dbPort);
//				}
//			}
//		}
//	}
//
//	// PickModelServer 选股模型  // 模型数据保存在CMainFrame吧
//	TiXmlElement* pPickModelTypeIDElement = pRootElement->FirstChildElement(KStrElementAttriPickModelType);
//	if( NULL != pPickModelTypeIDElement && NULL != pPickModelTypeIDElement->Value()  )
//	{
//		CMainFrame* pMainFrame = dynamic_cast<CMainFrame*>(AfxGetMainWnd());
//		if(NULL == pMainFrame)
//		{	
//			return;
//		}
//	
//		pMainFrame->m_vPickModelTypeGroup.clear();
//		const char *pValue = pPickModelTypeIDElement->Value();
//		ASSERT( 0 == strcmp(pPickModelTypeIDElement->Value(), KStrElementAttriPickModelType) );
//		TiXmlElement* pServerElement = pPickModelTypeIDElement->FirstChildElement("ModeTypeId");
//		for( ; NULL != pServerElement; pServerElement = pServerElement->NextSiblingElement("ModeTypeId"))
//		{
//			int iModelId  = -1;
//			const char *pValue  = pServerElement->Value();
//			const char* StrAddress = pServerElement->Attribute("ModeId");
//			if (NULL != StrAddress)
//			{
//				iModelId = atoi(StrAddress);
//				pMainFrame->m_vPickModelTypeGroup.push_back(iModelId);
//			}	
//		}
//	}
//}

bool32 CDlgNewLogin::GetUserInfoFromXml()
{
	// 从公共的config 目录下 Xml 文件中获得保存的用户信息:	
	CString StrPath = CPathFactory::GetUserInfoPath();
	
	if (StrPath.IsEmpty() )
	{
		return false;
	}
	
	string sPath = _Unicode2MultiChar(StrPath);
	const char* StrUserInfoPath = sPath.c_str();
	CCodeFile::DeCodeFile(StrUserInfoPath);
	
	TiXmlDocument myDocument = TiXmlDocument(StrUserInfoPath);
	
	if ( !myDocument.LoadFile() )
	{
		return false;
	}
	
	// XMLDATA
	TiXmlElement* pRootElement = myDocument.RootElement();
	if ( NULL == pRootElement )
	{
		return false;
	}
	
	// UserInfo
	pRootElement = pRootElement->FirstChildElement();
	if (NULL == pRootElement)
	{
		return false;
	}
	
	// User
	if ( NULL == pRootElement->Value() || 0 != strcmp(pRootElement->Value(), KStrElementNameUserInfo) )
	{
		// 没有用户信息就算了
		return true;
	}
	
	m_aUserInfo.RemoveAll();
	
	//
	TiXmlElement* pUserElement = pRootElement->FirstChildElement(KStrElementNameUser);
	
	if ( NULL == pUserElement || NULL == pUserElement->Value() || 0 != strcmp(pUserElement->Value(), KStrElementNameUser))
	{
		return true;
	}
	
	for ( ; NULL != pUserElement; pUserElement = pUserElement->NextSiblingElement(KStrElementNameUser) )
	{
		//
		const char* StrUserName = pUserElement->Attribute(KStrElementAttriUserName);
		const char* StrUserPwd  = pUserElement->Attribute(KStrElementAttriUserPwd);
		const char* StrBeSave   = pUserElement->Attribute(KStrElementAttriBeSave);
		const char* StrIndex	= pUserElement->Attribute(KStrElementAttriServerIndex);
		const char* StrBeLastUser =	pUserElement->Attribute(KStrElementAttriBeLastUser);
		const char* StrEnvIndex = pUserElement->Attribute(KStrElementAttriEnvIndex);
		const char* StrAtuoLogin = pUserElement->Attribute(KStrElementAttriAutoLogin);
		const char* StrSCToolBar = pUserElement->Attribute(KStrElementAttriSCToolBar);

		if ( NULL == StrUserName || strlen(StrUserName)==0 )
		{
			//空用户名
			continue;
		}
		
		// 名称
		TCHAR TStrUserName[1024];
		memset(TStrUserName, 0, sizeof(TStrUserName));
		MultiCharCoding2Unicode(EMCCUtf8, StrUserName, strlen(StrUserName), TStrUserName, sizeof(TStrUserName) / sizeof(TCHAR));
		
		// 密码
		TCHAR TStrUserPwd[1024];
		memset(TStrUserPwd, 0, sizeof(TStrUserPwd));
		if ( NULL!=StrUserPwd )
		{
			MultiCharCoding2Unicode(EMCCUtf8, StrUserPwd, strlen(StrUserPwd), TStrUserPwd, sizeof(TStrUserPwd) / sizeof(TCHAR));
		}
		
		T_UsersInfo	userInfo;
		
		userInfo.StrUserName	=	TStrUserName;
		userInfo.StrPassword	=	TStrUserPwd;
		userInfo.BeSavePassword	=	NULL==StrBeSave ? false : atoi(StrBeSave);
		userInfo.BeLastUser		=	NULL==StrBeLastUser ? false : atoi(StrBeLastUser);
		userInfo.IndexServer	=	NULL==StrIndex ? 0 : atoi(StrIndex);
		userInfo.EnvIndex  		=	NULL==StrEnvIndex ? false : atoi(StrEnvIndex);
		userInfo.IsAtutoLogin	=	NULL==StrAtuoLogin ? false : atoi(StrAtuoLogin);
		userInfo.IsShowSCToolBar=	NULL==StrSCToolBar ? false : atoi(StrSCToolBar);
		m_indexServerSel = userInfo.IndexServer;
		
		// 取用户权限:
		for (TiXmlElement* pUserRightElement = pUserElement->FirstChildElement(); NULL != pUserRightElement; pUserRightElement = pUserRightElement->NextSiblingElement() )
		{
			const char* pStrCode = pUserRightElement->Attribute(KStrElementAttriRightCode);			
			const char* pStrName = pUserRightElement->Attribute(KStrElementAttriRightName);			

			if ( NULL == pStrCode || NULL == pStrName )
			{
				continue;
			}

			//
			T_RightInfo stRightInfo;

			wstring strCode, strName;

			Utf8ToUnicode(pStrCode, strCode);
			Utf8ToUnicode(pStrName, strName);

			stRightInfo.iFunID = _ttoi(strCode.c_str());
	//		lstrcpy(stRightInfo.wszCode, strCode.c_str());
			lstrcpy(stRightInfo.wszName, strName.c_str());

			//
			userInfo.m_aUserRights.Add(stRightInfo);
		}
		
		m_aUserInfo.Add(userInfo);
	}

	CCodeFile::EnCodeFile(StrUserInfoPath);
	
	return true;
}

bool32 CDlgNewLogin::SetUserInfoToXml()
{
	// 保存最后一次登陆保存的用户信息:
	UpdateData(TRUE);
	
	if ( FALSE == m_bSaveUser)
	{
		DeleteUserInfoFromXml(m_StrUserName);
		return false;
	}

	CString StrPath = CPathFactory::GetUserInfoPath();
	string sPath = _Unicode2MultiChar(StrPath);	

	const char* StrUserInfoPath = sPath.c_str();
	CCodeFile::DeCodeFile(StrUserInfoPath);
	
	TiXmlDocument myDocument = TiXmlDocument(StrUserInfoPath);
	
	if ( !myDocument.LoadFile() )
	{
		return	SetDefaultUserInfoToXml();	
	}

	// UserInfo
	TiXmlElement*	pRootElement = myDocument.RootElement();
	if (NULL == pRootElement)
	{
		return	SetDefaultUserInfoToXml();
	}
	
	// User
	pRootElement	=	pRootElement->FirstChildElement(KStrElementNameUserInfo);
	if ( NULL == pRootElement->Value() || 0 != strcmp(pRootElement->Value(), KStrElementNameUserInfo) )
	{
		// 没有用户信息就算了
		return	SetDefaultUserInfoToXml();
	}

	TiXmlElement* pUserElement = pRootElement->FirstChildElement(KStrElementNameUser);
	
	int32 iServerIndex = m_indexServerSel;//m_CtrlComboHost.GetCurSel();	
	if ( iServerIndex < 0 || iServerIndex >= m_aHostInfo.GetSize()/*m_CtrlComboHost.GetCount() */)
	{
		iServerIndex = 0;
	}

	bool32	bModified	=	false;

	CString	StrPwd	=	m_StrPassword;
	if ( !m_bSavePwd )
	{
		//不保存密码
		StrPwd.Empty();	
	}

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
			char	tmpBuf[1024];
			
			Unicode2MultiCharCoding(EMCCUtf8, StrPwd, -1, tmpBuf, sizeof(tmpBuf));	//password
			pUserElement->SetAttribute(KStrElementAttriUserPwd, tmpBuf);

			CString	tmpStr;
			tmpStr.Format(_T("%d"), m_bSavePwd);			
			Unicode2MultiCharCoding(EMCCUtf8, tmpStr, -1, tmpBuf, sizeof(tmpBuf));	//save pwd
			pUserElement->SetAttribute(KStrElementAttriBeSave, tmpBuf);

			tmpStr.Format(_T("%d"), TRUE);			//lastUser
			Unicode2MultiCharCoding(EMCCUtf8, tmpStr, -1, tmpBuf, sizeof(tmpBuf));
			pUserElement->SetAttribute(KStrElementAttriBeLastUser, tmpBuf);

			tmpStr.Format(_T("%d"), iServerIndex);			//iServerIndex
			Unicode2MultiCharCoding(EMCCUtf8, tmpStr, -1, tmpBuf, sizeof(tmpBuf));
			pUserElement->SetAttribute(KStrElementAttriServerIndex, tmpBuf);

			//2013-7-4
			tmpStr.Format(_T("%d"), m_envIndex);			
			Unicode2MultiCharCoding(EMCCUtf8, tmpStr, -1, tmpBuf, sizeof(tmpBuf));	//save pwd
			pUserElement->SetAttribute(KStrElementAttriEnvIndex, tmpBuf);
			
			//lxp add
			tmpStr.Format(_T("%d"), m_bAutoLogin);			
			Unicode2MultiCharCoding(EMCCUtf8, tmpStr, -1, tmpBuf, sizeof(tmpBuf));	//auto login
			pUserElement->SetAttribute(KStrElementAttriAutoLogin, tmpBuf);

			bModified	=	true;  
		}
		else
		{
			//其它的需要改变lastUser属性 为 false，简单化，直接0
			pUserElement->SetAttribute(KStrElementAttriBeLastUser, "0");
		}

		//
		pUserElement	=	pUserElement->NextSiblingElement(KStrElementNameUser);
	}

	if ( !bModified )
	{
		//添加
		TiXmlElement	eleNew(KStrElementNameUser);
		CString	tmpStr;
		char	tmpBuf[1024];		//存在内存溢出的隐患

		//用户名
		Unicode2MultiCharCoding(EMCCUtf8, m_StrUserName, -1, tmpBuf, sizeof(tmpBuf));
		eleNew.SetAttribute(KStrElementAttriUserName, tmpBuf);

		//pwd
		Unicode2MultiCharCoding(EMCCUtf8, StrPwd, -1, tmpBuf, sizeof(tmpBuf));	//password
		eleNew.SetAttribute(KStrElementAttriUserPwd, tmpBuf);

		//besave
		_snprintf(tmpBuf, sizeof(tmpBuf), "%d", m_bSavePwd);
		eleNew.SetAttribute(KStrElementAttriBeSave, tmpBuf);

		//beLastUser
		eleNew.SetAttribute(KStrElementAttriBeLastUser, "1");

		//iServerIndex
		_snprintf(tmpBuf, sizeof(tmpBuf), "%d", iServerIndex);
		eleNew.SetAttribute(KStrElementAttriServerIndex, tmpBuf);
		
		//envindex 2013-7-4
		_snprintf(tmpBuf, sizeof(tmpBuf), "%d", m_bSaveUser);
		eleNew.SetAttribute(KStrElementAttriEnvIndex, tmpBuf);

		//bautologin
		_snprintf(tmpBuf, sizeof(tmpBuf), "%d", m_bAutoLogin);
		eleNew.SetAttribute(KStrElementAttriAutoLogin, tmpBuf);

		TiXmlNode *pnode = pRootElement->InsertEndChild( eleNew );
		ASSERT( NULL!=pnode );
	}

	bool bSave = myDocument.SaveFile();
	ASSERT( bSave );

	CCodeFile::EnCodeFile(StrUserInfoPath);

	return true;
}

bool32 CDlgNewLogin::SetDefaultUserInfoToXml()
{
	// 保存最后一次登陆保存的用户信息:
	UpdateData(TRUE);

	CString StrPath = CPathFactory::GetUserInfoPath();
	
	CString StrXml;
	StrXml  = L"<?xml version =\"1.0\" encoding=\"utf-8\" standalone=\"yes\"?> \n";
	StrXml += L"<XMLDATA version=\"1.0\" app = \"ggtong\" data = \"UserInfo\">\n";
	StrXml += L"<";
	StrXml += KStrElementNameUserInfo;
	StrXml += L">";
	
	CString StrUserInfo;
	
	int32 iServerIndex = m_indexServerSel;//m_CtrlComboHost.GetCurSel();	
	if ( iServerIndex < 0 || iServerIndex >= m_aHostInfo.GetSize() )
	{
		iServerIndex = 0;
	}

	//0001698 - 不能保存密码
	CString	StrPwd	=	m_StrPassword;
	if ( !m_bSavePwd )
	{
		//不保存密码
		StrPwd.Empty();
	}

	CString StrNameUser = (CString)KStrElementNameUser;
	CString StrAttriUserName = (CString)KStrElementAttriUserName;
	CString StrAttriUserPwd = (CString)KStrElementAttriUserPwd;
	CString StrAttriBeSave = (CString)KStrElementAttriBeSave;
	CString StrAttriBeLastUser = (CString)KStrElementAttriBeLastUser;
	CString StrAttriServerIndex = (CString)KStrElementAttriServerIndex;
	CString StrAttriEnvIndex = (CString)KStrElementAttriEnvIndex;
	CString StrAttriAutoLogin = (CString)KStrElementAttriAutoLogin;
	CString StrAttriSCToolBar = (CString)KStrElementAttriSCToolBar;
	
	StrUserInfo.Format( L"<%s  %s=\"%s\" %s=\"%s\" %s=\"%d\" %s=\"%d\" %s=\"%d\" %s=\"%d\" %s=\"%d\" %s=\"%d\"/>\n",	//0001698 - 调整format
		StrNameUser.GetBuffer(),
		StrAttriUserName.GetBuffer(),	  m_StrUserName.GetBuffer(),
		StrAttriUserPwd.GetBuffer(),	  StrPwd.GetBuffer(),		//0001698 - 不能保存密码
		StrAttriBeSave.GetBuffer(),		  m_bSavePwd,
		StrAttriBeLastUser.GetBuffer(),	  TRUE,			//0001698 这里为新加的属性，暗示最后一个登录的用户
		StrAttriServerIndex.GetBuffer(),  iServerIndex,
		StrAttriEnvIndex.GetBuffer(),     1,            //2013-7-4设置登录环境
		StrAttriAutoLogin.GetBuffer(),	  m_bAutoLogin,
		StrAttriSCToolBar.GetBuffer(),	  m_bShowSCToolBar);
	
	StrXml += StrUserInfo;
	StrXml += L"</";
	StrXml += KStrElementNameUserInfo;
	StrXml += L">\n";
	StrXml += L"</XMLDATA>";
	
	string sPath = _Unicode2MultiChar(StrPath);
	const char* StrUserInfoPath = sPath.c_str();
	SaveXmlFile(StrUserInfoPath, StrXml);
	CCodeFile::EnCodeFile(StrUserInfoPath);
	
	return true;
}

void CDlgNewLogin::DeleteUserInfoFromXml( const CString &StrUserName )
{
	CString StrPath = CPathFactory::GetUserInfoPath();

	string sPath = _Unicode2MultiChar(StrPath);
	const char* StrUserInfoPath = sPath.c_str();
	CCodeFile::DeCodeFile(StrUserInfoPath);
	
	TiXmlDocument myDocument = TiXmlDocument(StrUserInfoPath);
	
	if ( !myDocument.LoadFile() )
	{
		return;	
	}
	
	// UserInfo
	TiXmlElement*	pRootElement = myDocument.RootElement();
	if (NULL == pRootElement)
	{
		return;
	}
	
	// User
	pRootElement	=	pRootElement->FirstChildElement(KStrElementNameUserInfo);
	if ( NULL == pRootElement->Value() || 0 != strcmp(pRootElement->Value(), KStrElementNameUserInfo) )
	{
		// 没有用户信息就算了
		return;
	}
	
	TiXmlElement* pUserElement = pRootElement->FirstChildElement(KStrElementNameUser);
	while( NULL != pUserElement )
	{
		//先查找以前有这个的用户信息没
		//
		const char* StrUserName2 = pUserElement->Attribute(KStrElementAttriUserName);
		
		// 名称
		TCHAR TStrUserName[1024];
		memset(TStrUserName, 0, sizeof(TStrUserName));
		MultiCharCoding2Unicode(EMCCUtf8, StrUserName2, strlen(StrUserName2), TStrUserName, sizeof(TStrUserName) / sizeof(TCHAR));
		
		if ( StrUserName.Compare(TStrUserName) == 0 )
		{
			//找到了，删除可以了
			pRootElement->RemoveChild(pUserElement);
			break;
		}
		
		pUserElement	=	pUserElement->NextSiblingElement(KStrElementNameUser);
	}
	
	bool bSave = myDocument.SaveFile();
	ASSERT( bSave );
	
	CCodeFile::EnCodeFile(StrUserInfoPath);
}

bool32 CDlgNewLogin::GetProxyInfoFromXml(OUT T_ProxyInfo& ProxyInfo)
{
	m_bUseProxy = false;
	m_bGreenChannel = false;
	
	// 从私人目录下读取代理信息:
	if ( m_StrUserName.GetLength() <= 0 )
	{
		return false;
	}
	
	//
	UpdateData(true);
	
	CString StrPath = CPathFactory::GetProxyInfoPath(m_StrUserName);
	
	//	
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

	// ProxyUserPwd
	const char* StrGreenChannel  = pProxyElement->Attribute(KStrElementAttriGreenChannel);
	
	
	// 赋值:
	ProxyInfo.uiProxyType	  = atoi(StrProxyType);
	ProxyInfo.StrProxyAddress = StrProxyAddress;
	ProxyInfo.uiProxyPort	  = atoi(StrProxyPort);
	ProxyInfo.StrUserName	  = _A2W(StrProxyUserName);
	ProxyInfo.StrUserPwd	  = _A2W(StrProxyUserPwd);
	
	m_bUseProxy				  = atoi(StrUseProxy);

	if (StrGreenChannel)
	{
		m_bGreenChannel			  = atoi(StrGreenChannel);
	}
	

	return true;
}

void CDlgNewLogin::DrawPromptInfo( CDC &dc )
{
	/*
	if ( IsWindow(m_CtrlStaticPrompt.m_hWnd) && !m_CtrlStaticPrompt.IsWindowVisible() )
	{
		CRect rcText;
		m_CtrlStaticPrompt.GetWindowRect(rcText);
		ScreenToClient(rcText);
		CString StrText;
		m_CtrlStaticPrompt.GetWindowText(StrText);
		int iSave = dc.SaveDC();
		dc.SelectObject(m_CtrlStaticPrompt.GetFont());
		SendMessage(WM_CTLCOLORSTATIC, (WPARAM)dc.m_hDC, (LPARAM)m_CtrlStaticPrompt.m_hWnd);
		OnCtlColor(&dc, &m_CtrlStaticPrompt, CTLCOLOR_STATIC);
		dc.DrawText(StrText, rcText, DT_SINGLELINE | DT_VCENTER | DT_CENTER);
		dc.RestoreDC(iSave);
	}
	*/
}

void CDlgNewLogin::SetPromptInfo(const CString &StrPrompt)
{
	CString StrStep;
	int iLower, iUpper;
	iLower = iUpper = 0;
	m_CtrlProgressLogin.GetRange32(iLower, iUpper);
	if ( iUpper - iLower > 0 )
	{
		int32 iStep = m_CtrlProgressLogin.GetPos();
		if ( iStep > 0 && iStep <= iUpper)
		{
			StrStep.Format(_T(" (%d/%d)"), iStep, iUpper);
		}
	}
	m_CtrlStaticPrompt.SetWindowText(StrPrompt + StrStep);
	// 自己画
	if ( IsWindow(m_CtrlStaticPrompt.m_hWnd) && !m_CtrlStaticPrompt.IsWindowVisible() )
	{
		CRect rcText;
		m_CtrlStaticPrompt.GetWindowRect(rcText);
		ScreenToClient(rcText);
		InvalidateRect(rcText);		
	}
}

void CDlgNewLogin::SetPromptInfo2(const CString &StrPrompt)
{
	m_CtrlStaticPrompt2.SetWindowText(StrPrompt);
	m_CtrlStaticPrompt2.RedrawWindow();
}

void CDlgNewLogin::SetProgress(int32 iPos, int32 iRange)
{
	if (iRange >= 0)
	{
		m_CtrlProgressLogin.SetRange32(0, iRange);
	}
	
	m_CtrlProgressLogin.SetPos(iPos);
}

void CDlgNewLogin::StepProgress()
{
	m_CtrlProgressLogin.StepIt();
	
	int iLow, iHigh;
	m_CtrlProgressLogin.GetRange32(iLow,iHigh);

	if ( iLow == iHigh)
	{
		iHigh = 57;
		iLow  = 0;
	}
}

bool32 CDlgNewLogin::ConnectAuth(int32 iIndexServer)
{
	// 要是数组越界	
	int32 iIndex;

	//
	if ( -1 == iIndexServer )
	{
		iIndex = m_iIndexCurAuth;
	}
	else
	{
		iIndex = iIndexServer;
	}

	if ( iIndex < 0 || m_aHostInfo.GetSize() <= 0 || iIndex >= m_aHostInfo.GetSize() )
	{
		//ASSERT(0);
		return false;
	}
	
	//	
	/*if ( !m_bFirstTime )
	{
		ReLoadAuthModule();
	}
	
	if ( m_bFirstTime )
	{
		m_bFirstTime = false;
	}*/

	CGGTongApp *pApp  =  (CGGTongApp*) AfxGetApp();	
	CGGTongDoc *pDoc = pApp->m_pDocument;

	if ( NULL == pDoc )
	{
		return false;
	}

	pDoc->m_bAutoLogin = m_bAutoLogin;

	T_HostInfo stHostInfo = m_aHostInfo[iIndex];

	// 获取代理的信息:
	T_ProxyInfo ProxyInfo;	
	bool32 bValidProxyInfo = GetProxyInfoFromXml(ProxyInfo);
	
	// 
	SetPromptInfo(L"提示：认证中...");
	SetPromptInfo2(L"");

	//		
	m_bCancelAuth = false;

	//
	pDoc->m_eNetType = (E_NetType)stHostInfo.uiNetType;
	COptimizeServer::Instance()->SetNetType(pDoc->m_eNetType);

	vector<unsigned int> vtPort;
	pDoc->m_pAutherManager->GetInterface()->ConnectServer(stHostInfo.StrHostAddress, vtPort);

	if (m_bGreenChannel)
	{
		//pDoc->m_pAutherManager->GetInterface()->AddGreenChannel(m_vUiServerBackup);
	}

	m_strSelService = stHostInfo.StrHostAddress;
	
	if ( ProxyInfo.uiProxyType <= 0 || ProxyInfo.uiProxyType > 3 )
	{
		// 不支持sock5??
		bValidProxyInfo = false;
	}

	if ( bValidProxyInfo && m_bUseProxy )
	{
		pDoc->m_ProxyInfo.m_eProxyType = (CProxyInfo::E_ProxyType)ProxyInfo.uiProxyType;
		pDoc->m_ProxyInfo.m_StrProxyAddr = ProxyInfo.StrProxyAddress;
		pDoc->m_ProxyInfo.m_iProxyPort	 = ProxyInfo.uiProxyPort;
		pDoc->m_ProxyInfo.m_StrProxyUser = ProxyInfo.StrUserName;
		pDoc->m_ProxyInfo.m_StrProxyPassword	= ProxyInfo.StrUserPwd;
	}
	else
	{
		// 需要重置代理信息
		pDoc->m_ProxyInfo.m_eProxyType = CProxyInfo::EPTNone;
	}
	
	_MYTRACE(L"----------------登录状态跟踪-------------------");
	_MYTRACE(L"开始认证 - 设置认证参数");


	_MYTRACE(L"开始认证 - 准备认证");
	{
		if ( bValidProxyInfo && m_bUseProxy )
		{
			_MYTRACE(L"认证参数 - %s:%d[代理类型:%d,%s,%d],%s,****", stHostInfo.StrHostAddress.GetBuffer(), (uint16)stHostInfo.uiHostPort, ProxyInfo.uiProxyType, ProxyInfo.StrProxyAddress.GetBuffer(), ProxyInfo.uiProxyPort, m_StrUserName.GetBuffer());
		}
		else
		{
			_MYTRACE(L"认证参数 - %s:%d,%s,****", stHostInfo.StrHostAddress.GetBuffer(), (uint16)stHostInfo.uiHostPort, m_StrUserName.GetBuffer());
		}
	}

	int32 iPwdLength = m_StrPassword.GetLength();
	if (32 != iPwdLength)		// 不等于32md5加密， 等于32已经进行md5加密
	{
		string sPwd, sPwdMd5;
		UnicodeToUtf8(m_StrPassword, sPwd);

		MD5(sPwd, sPwdMd5);
		m_StrPassword = sPwdMd5.c_str();
		UpdateData(false);
	}

	pDoc->m_pAutherManager->GetInterface()->Authorize(m_StrUserName, m_StrPassword, CConfigInfo::Instance()->GetOrgKey(), CConfigInfo::Instance()->GetVersionNo());

	_MYTRACE(L"开始认证 - 已调用认证");

	pDoc->m_pAbsCenterManager->SetLoginState(ELSAuthing);
	SetControlsState(pDoc->m_pAbsCenterManager->GetLoginState());

	

	//
	if ( -1 == iIndexServer )
	{
		++m_iIndexCurAuth;
	}
	return true;
}

void CDlgNewLogin::ReLoadAuthModule()
{
	CGGTongApp *pApp  =  (CGGTongApp*)AfxGetApp();	
	CGGTongDoc *pDoc = pApp->m_pDocument;
	
	if ( NULL == pDoc )
	{
		return;
	}
	CAbsCenterManager *pAbsCenterManager = pDoc->m_pAbsCenterManager;
	if (!pAbsCenterManager)
	{
		return;
	}
	
	// 重新认证
	if (NULL != pDoc->m_pAutherManager)
	{		
		pAbsCenterManager->SetLoginState(ELSNotLogin) ;
		
		pDoc->m_pAutherManager->CloseInstance();
		DEL(pDoc->m_pAutherManager);
	}
	
	// 重新装载资源
	pDoc->m_pAutherManager = new CProxyAuthClientHelper();
	ASSERT( NULL != pDoc->m_pAutherManager);
	
	if ( !pDoc->m_pAutherManager->CreateInstance() )		
	{
		//ASSERT(0);
		return;
	}			
	
	// 创建时就已经设置了通知
	//pDoc->m_pAutherManager->GetInterface()->SetNotifyObj(pDoc->m_pAbsCenterManager);
	pDoc->m_pAbsCenterManager->SetServiceDispose(pDoc->m_pAutherManager->GetInterface()->GetServiceDispose());
	//pDoc->m_pAbsDataManager->SetServiceDispose(pDoc->m_pAutherManager->GetInterface()->GetServiceDispose());
}

void CDlgNewLogin::ReSetLogData()
{
	// 可能存在重复登陆的过程, 所以每次登录前, 清空所有登录相关信息, 保证每次都是完整的重新登录流程
	m_iIndexCurAuth = 0;	
	SetProgress(0);
	SetPromptInfo(L"未登录");
	
	CGGTongDoc *pDoc  =  (CGGTongDoc*)AfxGetDocument();		
	if (NULL != pDoc && NULL != pDoc->m_pAbsCenterManager)
	{
		pDoc->m_pAbsCenterManager->ReSetLogData();
	}	
	

}

void CDlgNewLogin::AddButton(LPRECT lpRect, Image *pImage, UINT nCount, UINT nID, LPCTSTR lpszCaption)
{
	ASSERT(pImage);
	CNCButton btnControl; 
	btnControl.CreateButton(lpszCaption, lpRect, this, pImage, nCount, nID);
	btnControl.SetParentFocus(FALSE);

	if (ID_LOGIN_CONNECT == nID)
	{
		btnControl.SetFont(_T("宋体"),12, FontStyleBold);
	}
	else if (ID_LOGIN_REGISTERED == nID)
	{
		btnControl.SetFont(_T("宋体"),10.5, FontStyleBold);
	}
	m_mapBtn[nID] = btnControl;
}

int	 CDlgNewLogin::TButtonHitTest(CPoint point)
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

LRESULT CDlgNewLogin::OnNcHitTest(CPoint point) 
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

////////////////////////////////////
auth::PROXYTYPE MyNewShowProxyTypeToAuthProxyType( UINT uMyType )
{
	auth::PROXYTYPE authProxyType = NONE;
	switch ( uMyType )
	{
	case 1:				// HTTP
		{
			authProxyType = HTTP;
		}
		break;
	case 2:				//SOCK4
		{
			authProxyType = SOCK4;
		}
		break;
	case 3:				//SOCK5
		{
			authProxyType = SOCK5;
		}
		break;
	default:
		break;
	}
	return authProxyType;
}

LRESULT CDlgNewLogin::OnMsgViewDataOnAuthSuccess(WPARAM wParam, LPARAM lParam)
{
	CGGTongApp *pApp  =  (CGGTongApp*) AfxGetApp();	
	CGGTongDoc *pDoc = pApp->m_pDocument;
	CAbsCenterManager *pAbsCenterManager = pDoc->m_pAbsCenterManager;
	if (!pAbsCenterManager)
	{
		return 0;
	}
	ASSERT(NULL != pDoc->m_pAutherManager);
	
	// 
	pDoc->m_bNewUser = m_bNewUser;		// 记录这个登录的用户是否为新用户
	
	//
	SetControlsState(pAbsCenterManager->GetLoginState());
	
	pAbsCenterManager->SetKickOutState(false);
	//
	SetPromptInfo(L"提示：认证成功！");
	
	// 保存XML
	if ( !m_bDefaultUser )
		SetUserInfoToXml();
	
	// 认证成功后， 复制公共目录下的版面文件到私有目录下
	CGGTongApp::CopyPublicWCVFilesToPrivate(m_StrUserName);
	bool32 bUserPseudo;									// 重新扫描一次，以更改wsp中文件路径表示方式
	pApp->ScanAvaliableWspFile(m_StrUserName, bUserPseudo);
	
	//
	if (0 == m_StrUserName.CompareNoCase(pApp->m_pConfigInfo->m_StrDefUser) && 0 == m_StrPassword.CompareNoCase(pApp->m_pConfigInfo->m_StrDefPwd))
	{
		pApp->m_bCustom = true;
	}
	
	int iRange = (int32)lParam;
	if (iRange > 0)
	{
		SetProgress((int32)wParam,iRange);
	}
	
	return 0;
}

// 配置信息全部返回
LRESULT CDlgNewLogin::OnMsgViewDataConfigSuccessTransToMain(WPARAM wParam, LPARAM lParam)
{
	CGGTongApp *pApp  =  (CGGTongApp*) AfxGetApp();	
	if (NULL == pApp || NULL == pApp->m_pDocument || NULL == pApp->m_pConfigInfo || NULL == pApp->m_pDocument->m_pAutherManager)
	{
		return 0;
	}
	
	CGGTongDoc *pDoc = pApp->m_pDocument;
	CAbsCenterManager *pAbsCenterManager = pDoc->m_pAbsCenterManager;
	if (!pAbsCenterManager)
	{
		return 0;
	}
	{
		//	离线登录已成功，可以正常流程了。
		// 保存工具栏文件版本号
		CString StrVer = pDoc->m_pAutherManager->GetInterface()->GetServiceDispose()->GetToolBarVer();
		if (!StrVer.IsEmpty())
		{
			pApp->m_pConfigInfo->m_StrToolBarVer = StrVer;
		}

		bool bUpDate = pDoc->m_pAutherManager->GetInterface()->GetServiceDispose()->IsToolBarModify();
		if(pApp && pApp->m_pConfigInfo->m_bXmlFileUpdate && bUpDate)
		{
			CString StrPath = CPathFactory::GetToolBarConfigFilePath();
			if ( StrPath.IsEmpty() )
			{
				return false;
			}

			string sPath = _Unicode2MultiChar(StrPath);
			const char* strPath = sPath.c_str();
			Encrypt(strPath);
		}

		// 配置信息返回后，才开始连接行情
		
		ASSERT(NULL != pDoc->m_pAutherManager);

		CString StrProduct = CConfigInfo::Instance()->GetAuthKey();
		pAbsCenterManager->SetUserInfo(m_StrUserName, m_StrPassword, StrProduct);
		//	获取市场信息
		pAbsCenterManager->OnAuthSuccessMain();
		SetPromptInfo(L"连接行情服务器...");
		pAbsCenterManager->InitialDataServers(pDoc->m_ProxyInfo);
	}

	return 0;
}

LRESULT CDlgNewLogin::OnMsgViewDataAuthSuccessTransToMain(WPARAM wParam, LPARAM lParam)
{	
	ReqCommConfigInfo();

	return 0;
}

LRESULT CDlgNewLogin::OnMsgViewDataOnAuthFail(WPARAM wParam, LPARAM lParam)
{
	CString *pStrMsg = (CString *)wParam;
	ASSERT(NULL != pStrMsg);

	E_ErrType eErrType = (E_ErrType)lParam;

	/*
	{
		CString StrTmp;
		StrTmp.Format(_T("认证失败 - %d:%s"), eErrType, pStrMsg->operator LPCTSTR());
		XLTraceFile::GetXLTraceFile(KStrLoginLogFileName).TraceWithTimestamp(StrTmp);
	}
	*/

	_MYTRACE(L"认证失败 - %d:%s", eErrType, pStrMsg->operator LPCTSTR());

	// 
	CGGTongApp *pApp  =  (CGGTongApp*) AfxGetApp();	
	CGGTongDoc *pDoc = pApp->m_pDocument;
	CAbsCenterManager *pAbsCenterManager = pDoc->m_pAbsCenterManager;
	if (!pAbsCenterManager)
	{
		return 0;
	}
	
	if ( !pApp->m_pConfigInfo->m_StrLoginErrorTip.IsEmpty() )
	{
		// 使用自定义的提示字串
		SetPromptInfo2(pApp->m_pConfigInfo->m_StrLoginErrorTip);
	}
	
	if ( !m_bCancelAuth )
	{
		SetPromptInfo(*pStrMsg);
	}
	
	if ( enETOther == eErrType )
	{
		pAbsCenterManager->SetLoginState(ELSAuthFail);
		SetControlsState(pAbsCenterManager->GetLoginState());		
	}
	else if ( enETNetErr == eErrType ) // 只有在网络断了的情况下才请求下一个	
	{
		if ( !m_bCancelAuth && !ConnectAuth() )
		{
			//
			pAbsCenterManager->SetLoginState(ELSAuthFail);
			SetControlsState(pAbsCenterManager->GetLoginState());	
		}

	}
	else
	{
		pAbsCenterManager->SetLoginState(ELSAuthFail);
		SetControlsState(pAbsCenterManager->GetLoginState());	
	}

	CString StrPwdErr = _T("用户名或密码错误");
	CString StrSub = _T("");
	StrSub = _tcsstr(*pStrMsg, StrPwdErr);
	if (!StrSub.IsEmpty())
	{
		// 密码错误，清空密码
		GetDlgItem(IDC_EDIT_PWD)->SetWindowText(_T(""));
		GetDlgItem(IDC_EDIT_PWD)->SetFocus();
	}
	
	//DataCenterFree(pStrMsg);
	if (pStrMsg)
	{
		delete pStrMsg;
		pStrMsg = NULL;
	}

	return 0;
}

LRESULT CDlgNewLogin::OnMsgViewDataOnMessage(WPARAM wParam, LPARAM lParam)
{
	//
	CString *pStrMsg = (CString *)wParam;
	ASSERT(NULL != pStrMsg);

	SetPromptInfo(*pStrMsg);
	StepProgress();

	if (pStrMsg)
	{
		delete pStrMsg;
		pStrMsg = NULL;
	}
	//DataCenterFree(pStrMsg);
	return 0;
}

LRESULT CDlgNewLogin::OnMsgViewDataOnAllMarketInitializeSuccess(WPARAM wParam, LPARAM lParam)
{
	StepProgress();
	SetPromptInfo(L"提示：同步所有市场完成！");

	CGGTongDoc *pDocument = AfxGetDocument();			
	ASSERT(NULL!= pDocument);				

	CAbsCenterManager *pAbsCenterManager = pDocument->m_pAbsCenterManager;
	if (!pAbsCenterManager)
	{
		return 0;
	}
	ASSERT(NULL!= pAbsCenterManager);
	
	// 非游客登录更新自选股
	if (!m_bDefaultUser)
	{
		pAbsCenterManager->UpdateUserBlock();
	}
	
	pAbsCenterManager->SetLoginHWnd(NULL);
	EndDialog(IDOK);
	return 0;
}

LRESULT CDlgNewLogin::OnMsgViewDataOnDataServiceConnected(WPARAM wParam, LPARAM lParam)
{
	CGGTongApp *pApp = (CGGTongApp *)AfxGetApp();
	CGGTongDoc *pDocument = pApp->m_pDocument;			
	ASSERT(NULL!= pDocument);				
	
	CAbsCenterManager *pAbsCenterManager = pDocument->m_pAbsCenterManager;
	if (!pAbsCenterManager)
	{
		return 0;
	}
	ASSERT(NULL!= pAbsCenterManager);
	
	pAbsCenterManager->OnMsgServiceConnected((int32)wParam);
	
	return 0;
}

LRESULT CDlgNewLogin::OnMsgViewDataOnDataServiceDisconnected(WPARAM wParam, LPARAM lParam)
{
	CGGTongApp *pApp = (CGGTongApp *)AfxGetApp();
	CGGTongDoc *pDocument = pApp->m_pDocument;			
	ASSERT(NULL!= pDocument);				
	
	CAbsCenterManager *pAbsCenterManager = pDocument->m_pAbsCenterManager;
	if (!pAbsCenterManager)
	{
		return 0;
	}
	
	pAbsCenterManager->OnMsgServiceDisconnected((int32)wParam);
	
	return 0;
}

LRESULT CDlgNewLogin::OnMsgViewDataOnDataRequestTimeOut(WPARAM wParam, LPARAM lParam)
{
	CGGTongApp *pApp = (CGGTongApp *)AfxGetApp();
	CGGTongDoc *pDocument = pApp->m_pDocument;			
	ASSERT(NULL!= pDocument);				
	
	CAbsCenterManager *pAbsCenterManager = pDocument->m_pAbsCenterManager;
	if (!pAbsCenterManager)
	{
		return 0;
	}
	
	pAbsCenterManager->OnMsgRequestTimeOut((int32)wParam);
	
	return 0;
}

LRESULT CDlgNewLogin::OnMsgViewDataOnDataCommResponse(WPARAM wParam, LPARAM lParam)
{
	CGGTongApp *pApp = (CGGTongApp *)AfxGetApp();
	CGGTongDoc *pDocument = pApp->m_pDocument;			
	ASSERT(NULL!= pDocument);				
	
	CAbsCenterManager *pAbsCenterManager = pDocument->m_pAbsCenterManager;
	if (!pAbsCenterManager)
	{
		return 0;
	}
	
	pAbsCenterManager->OnMsgCommResponse();
	
	return 0;
}

LRESULT CDlgNewLogin::OnMsgViewDataOnDataServerConnected(WPARAM wParam, LPARAM lParam)
{	
	SetPromptInfo(L"连接服务器成功, 开始请求市场信息...");		
	SetPromptInfo2(L"直连行情服务器成功!");	

	int32 iCommunicationId = int32(wParam);
	
	CGGTongApp *pApp = (CGGTongApp *)AfxGetApp();
	CGGTongDoc *pDocument = pApp->m_pDocument;			
	ASSERT(NULL!= pDocument);				
	
	CAbsCenterManager *pAbsCenterManager = pDocument->m_pAbsCenterManager;
	if (!pAbsCenterManager)
	{
		return 0;
	}

	_MYTRACE(L"登陆窗口收到ViewData 发送的服务器 %d 连接成功消息, 转回ViewData处理:", iCommunicationId);
	pAbsCenterManager->OnMsgServerConnected(iCommunicationId);
	
	return 0;
}

LRESULT CDlgNewLogin::OnMsgViewDataOnDataServerDisconnected(WPARAM wParam, LPARAM lParam)
{
	int32 iCommunicationId = int32(wParam);

	CGGTongApp *pApp = (CGGTongApp *)AfxGetApp();
	CGGTongDoc *pDocument = pApp->m_pDocument;			
	ASSERT(NULL!= pDocument);				
	
	CAbsCenterManager *pAbsCenterManager = pDocument->m_pAbsCenterManager;
	ASSERT(NULL!= pAbsCenterManager);
	if (!pAbsCenterManager)
	{
		return 0;
	}
	
	pAbsCenterManager->OnMsgServerDisconnected(iCommunicationId);
	
	return 0;
}

LRESULT CDlgNewLogin::OnMsgViewDataOnDataServerLongTimeNoRecvData(WPARAM wParam, LPARAM lParam)
{
	CGGTongApp *pApp = (CGGTongApp *)AfxGetApp();
	CGGTongDoc *pDocument = pApp->m_pDocument;			
	ASSERT(NULL!= pDocument);				
	
	CAbsCenterManager *pAbsCenterManager = pDocument->m_pAbsCenterManager;
	ASSERT(NULL!= pAbsCenterManager);
	if (!pAbsCenterManager)
	{
		return 0;
	}
	
	pAbsCenterManager->OnMsgServerLongTimeNoRecvData((int32)wParam, (E_ReConnectType)lParam);
	
	return 0;
}

LRESULT CDlgNewLogin::OnMsgViewDataOnBlockFileTranslateTimeout( WPARAM w, LPARAM l )
{
	CGGTongApp *pApp = (CGGTongApp *)AfxGetApp();
	CGGTongDoc *pDocument = pApp->m_pDocument;			
	ASSERT(NULL!= pDocument);				
	
	CAbsCenterManager *pAbsCenterManager = pDocument->m_pAbsCenterManager;
	ASSERT(NULL!= pAbsCenterManager);
	if (!pAbsCenterManager)
	{
		return 0;
	}
	
	pAbsCenterManager->OnBlockFileTranslateTimeout();
	
	return 0;
}

LRESULT CDlgNewLogin::OnMsgDllTraceLog(WPARAM w, LPARAM l)
{
	CString* pStrLog = (CString*)l;
	
	CString StrLog = *pStrLog;
	_MYTRACE(StrLog);
	
	DEL(pStrLog);
	
	return 0;
}

void CDlgNewLogin::OnTimer(UINT nIDEvent)
{
	if ( KiTimerIdPing == nIDEvent )
	{
		COptimizeServer::Instance()->OnMyTimer(nIDEvent);
	}
	
	//自动登录
	if( m_nAutoLoginTimer == nIDEvent )
	{
		if( m_nAutoLoginTimer)
		{
			KillTimer(m_nAutoLoginTimer);
			m_nAutoLoginTimer = 0;
		}
		if( m_bAutoLogin )
		{
			PostMessage(WM_COMMAND,MAKEWPARAM(ID_LOGIN_CONNECT,BN_CLICKED),0);
		}
	}
	
	CDialog::OnTimer(nIDEvent);
}

void CDlgNewLogin::SetNet()
{
	UpdateData(true);
	
	CDlgConnectSet dlgConnectSet;	
	
	T_ProxyInfo ProxyInfo;
		
	if (GetProxyInfoFromXml(ProxyInfo))
	{
		dlgConnectSet.SetInitialProxyInfo(ProxyInfo);
	}
	
	int32 iIndexHost = 0;
	
	for (int i=0; i < m_aUserInfo.GetSize() ; i++)
	{
		if ( m_StrUserName.Compare(m_aUserInfo[i].StrUserName) == 0 )
		{
			iIndexHost = m_aUserInfo[i].IndexServer;
			break;
		}
	}
	
	dlgConnectSet.SetInitialInfo(m_StrUserName, m_bUseProxy, iIndexHost, m_bManualOptimaze, m_bGreenChannel);
	if(dlgConnectSet.DoModal())
	{
		GetUserInfoFromXml();
		m_bManualOptimaze = dlgConnectSet.m_bCheckManual;
		m_indexServerSel = dlgConnectSet.m_iHostIndex;
	}
}

BOOL CDlgNewLogin::PreTranslateMessage(MSG* pMsg) 
{
	if ( WM_KEYDOWN == pMsg->message && VK_RETURN == pMsg->wParam)
	{
		OnCommand(ID_LOGIN_CONNECT, 0);
		return TRUE;
	}
	
	return BaseDialog::PreTranslateMessage(pMsg);
}

void CDlgNewLogin::OnButtonReg() 
{
	CGGTongApp* pApp = (CGGTongApp*)AfxGetApp();
	if (NULL == pApp)
	{
		return;
	}
	
	//
	CString StrUrl = pApp->m_pConfigInfo->m_StrRegUrl;
	CString StrName = pApp->m_pConfigInfo->m_StrBtName;
	if (0 >= pApp->m_pConfigInfo->m_StrBtName.GetLength())
	{
		StrName = L"用户注册";
	}

	bool32 bShowIE = FALSE;
	int32 iSize = pApp->m_pConfigInfo->m_aWndSize.GetSize();
	int32 iWidth = 1024, iHeight = 700;
	for (int32 i=0; i<iSize; i++)
	{
		CWndSize wnd = pApp->m_pConfigInfo->m_aWndSize[i];
		if (L"注册" == wnd.m_strID)
		{
			StrName = wnd.m_strTitle;
			iWidth = wnd.m_iWidth;
			iHeight = wnd.m_iHeight;

			if (0 == iWidth || 0 == iHeight)
			{
				bShowIE = TRUE;
			}
			break;
		}
	}

	if (bShowIE)
	{
		ShellExecute(0, L"open", StrUrl, NULL, NULL, SW_NORMAL);
	}
	else
	{
		if (m_pDlgComment != NULL)
		{
			m_pDlgComment->ShowDlgIEWithSize(StrName, StrUrl, CRect(0, 0, iWidth, iHeight));
		}
	}
}

void CDlgNewLogin::OnButtonVisitor() 
{
	CGGTongApp* pApp = (CGGTongApp*)AfxGetApp();
	if (NULL == pApp)
	{
		return;
	}
	
	if (pApp->m_pConfigInfo->m_StrDefUser.GetLength() <= 0 || pApp->m_pConfigInfo->m_StrDefPwd.GetLength() <= 0)
	{
		MessageBox(L"游客账户非法!", pApp->m_pszAppName, MB_ICONINFORMATION);
		return;
	}
	
	if (pApp->m_pConfigInfo->m_StrDefUser == _T("#"))
	{
		MessageBox(L"现不支持游客登录，请点击注册，使用注册帐号登录!", pApp->m_pszAppName, MB_ICONINFORMATION);
		return;
	}


	//
	
	
	m_StrUserName = pApp->m_pConfigInfo->m_StrDefUser;
	m_StrPassword = pApp->m_pConfigInfo->m_StrDefPwd;
	UpdateData(FALSE);
	
	
	//
	Connect(true);
}



void CDlgNewLogin::OnButtonOffLine()
{
	CGGTongApp::m_bOffLine = true;

	CGGTongApp *pApp  =  (CGGTongApp*) AfxGetApp();
	if(NULL == pApp)
	{
		return;
	}

	CGGTongDoc *pDoc = pApp->m_pDocument;

	if ( NULL == pDoc || NULL == pDoc->m_pAbsCenterManager )
	{
		ASSERT(0);
		return;
	}

	CAbsCenterManager *pAbsCenterManager = pDoc->m_pAbsCenterManager;

	pAbsCenterManager->SetOffLineData(true);

	
	T_UsersInfo stUserForOffLine;
	bool32 bValid = false;

	for ( int32 i = 0; i < m_aUserInfo.GetSize(); i++ )
	{
		if ( m_aUserInfo[i].BeLastUser)
		{
			bValid = true;
			stUserForOffLine = m_aUserInfo[i];
			break;
		}
	}

	//

	if ( !bValid )
	{		
		SetPromptInfo( _T("未登录用户，禁止进行脱机登录！"));
		return;
	}

	
	CString StrErr;
	//
	if ( bValid )
	{
		SetPromptInfo(L"脱机数据初始化...");
		SetControlsState(ELSLogining);		
		bValid = pDoc->m_pAbsCenterManager->OffLineInitial(stUserForOffLine.StrUserName,  StrErr);		
	}

	if ( !bValid )
	{
		SetControlsState(ELSLoginFail);
		if ( StrErr.IsEmpty() )
		{
			StrErr = _T("脱机登录失败！");
		}
		SetPromptInfo(StrErr);
		return;
	}

	// 初始化离线的版本等信息
	//CPluginFuncRight::Instance().InitializeCurrentSubVersion();
	//
	EndDialog(IDOK);
}



void CDlgNewLogin::OnBnClickedBtnPasswordRetake()
{
    // TODO: 在此添加控件通知处理程序代码
    CGGTongApp* pApp = (CGGTongApp*)AfxGetApp();
    if (NULL == pApp)
    {
        return;
    }

    //
    CString StrUrl = pApp->m_pConfigInfo->m_StrZHMMUrl;
    CString StrName = pApp->m_pConfigInfo->m_StrZHMMBtName;
    if (0 >= pApp->m_pConfigInfo->m_StrZHMMBtName.GetLength())
    {
        StrName = L"找回密码";
    }

    bool32 bShowIE = FALSE;
    int32 iSize = pApp->m_pConfigInfo->m_aWndSize.GetSize();
    int32 iWidth = 1024, iHeight = 700;
    for (int32 i=0; i<iSize; i++)
    {
        CWndSize wnd = pApp->m_pConfigInfo->m_aWndSize[i];
        if (L"找回密码" == wnd.m_strID)
        {
            StrName = wnd.m_strTitle;
            iWidth = wnd.m_iWidth;
            iHeight = wnd.m_iHeight;

            if (0 == iWidth || 0 == iHeight)
            {
                bShowIE = TRUE;
            }
            break;
        }
    }

    if (bShowIE)
    {
        ShellExecute(0, L"open", StrUrl, NULL, NULL, SW_NORMAL);
    }
    else
    {
		if (m_pDlgComment != NULL)
		{
			m_pDlgComment->ShowDlgIEWithSize(StrName, StrUrl, CRect(0, 0, iWidth, iHeight));
		}
    }
}

void CDlgNewLogin::Connect(bool32 bCustom)
{
	UpdateData(true);
	// 登录
	CGGTongApp *pApp  =  (CGGTongApp*) AfxGetApp();	
	CGGTongDoc *pDoc = pApp->m_pDocument;
	CAbsCenterManager *pAbsCenterManager = pDoc->m_pAbsCenterManager;
	
	if (NULL == pApp || NULL == pApp->m_pDocument || NULL == pApp->m_pDocument->m_pAbsCenterManager)
	{
		ASSERT(0);
		return ;
	}

	//	大市场数据来源于脱机文件/未成功获取到网络市场
	if (pDoc->m_pAbsCenterManager->IsOffLineLogin())
	{
		OnButtonOffLine();
		return;
	}
	pApp->m_bCustom = bCustom;
	m_bDefaultUser = bCustom;
	// 设置一次显示工具栏标志
	for (int i=0; i < m_aUserInfo.GetSize() ; i++)
	{
		if ( m_StrUserName.Compare(m_aUserInfo[i].StrUserName) == 0 )
		{
			T_UsersInfo st;
			st = m_aUserInfo[i];
			pDoc->m_bShowSCToolBar = m_aUserInfo[i].IsShowSCToolBar;
			break;
		}
	}

	// 获取代理的信息:
	T_ProxyInfo ProxyInfo;	
	CGGTongApp::m_bOffLine = false; 
	//	主框架中的反初始化动作
	{
		CMainFrame* pMainFrame = dynamic_cast<CMainFrame*>(AfxGetMainWnd());
		if(NULL != pMainFrame)
		{	
			pMainFrame->Uninit();
		}
	}
	
	//
	ReSetLogData();
	ASSERT(NULL != pDoc->m_pAutherManager);
	
	if (ELSNotLogin	!= pAbsCenterManager->GetLoginState() &&
		ELSAuthFail	!= pAbsCenterManager->GetLoginState() &&
		ELSLoginFail != pAbsCenterManager->GetLoginState())
	{
		//ASSERT(0);
		return; //CDialog::OnCommand(wParam, lParam);
	}
	
	// 

	
	// 判断用户名,密码
	if (!bCustom && (m_StrUserName.GetLength() <= 0 || m_StrPassword.GetLength() <= 0))
	{
		SetPromptInfo(L"请输入账号/密码");
		return; //CDialog::OnCommand(wParam, lParam);
	}
	
	// 获取服务器
	int32 iCurHost = m_indexServerSel;//m_CtrlComboHost.GetCurSel();
	
	if (0 > iCurHost)
	{
		MessageBox(L"请选择服务器", L"提示");
		return; //CDialog::OnCommand(wParam, lParam);
	}
	
	// 如果不匹配, 默认取第一台
	if (iCurHost >= m_aHostInfo.GetSize())
	{
		iCurHost = 0;
		m_indexServerSel = 0;
	}
	
	T_HostInfo stHostInfo = m_aHostInfo[iCurHost];
	
	// 设置优选主站的模式
	COptimizeServer::Instance()->SetSortMode(!m_bManualOptimaze);
	SetPromptInfo(L"提示：认证中...");
	ConnectAuth(iCurHost);
//	RedrawWindow();
}

bool32	CDlgNewLogin::SetUserInfoStatus(CString strUserName,BOOL bAutoLogin,BOOL bManualOptimaze)
{
	CString StrPath = CPathFactory::GetUserInfoPath();
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
	
	bool32	bModified	=	false;
	while( NULL != pUserElement )
	{
		//先查找以前有这个的用户信息没
		//
		const char* StrUserName = pUserElement->Attribute(KStrElementAttriUserName);
		
		// 名称
		TCHAR TStrUserName[1024];
		memset(TStrUserName, 0, sizeof(TStrUserName));
		MultiCharCoding2Unicode(EMCCUtf8, StrUserName, strlen(StrUserName), TStrUserName, sizeof(TStrUserName) / sizeof(TCHAR));
		
		if ( strUserName.Compare(TStrUserName) == 0 )
		{
			//找到了，改变一下属性就可以了
			char	tmpBuf[1024];
			CString StrPwd = _T("");
			
			//
			CString	tmpStr;
			tmpStr.Format(_T("%d"), bAutoLogin);			
			Unicode2MultiCharCoding(EMCCUtf8, tmpStr, -1, tmpBuf, sizeof(tmpBuf));	//auto login
			pUserElement->SetAttribute(KStrElementAttriAutoLogin, tmpBuf);
			
			bModified	=	true;
		}
		//
		pUserElement	=	pUserElement->NextSiblingElement(KStrElementNameUser);
	}
	
	if ( !bModified )
	{
		return false;
	}
	
	bool bSave = myDocument.SaveFile();
	ASSERT( bSave );
	
	CCodeFile::EnCodeFile(StrUserInfoPath);
	
	return true;
}



bool32	CDlgNewLogin::SetUserToolBarStatus(CString strUserName,BOOL bShowSCToolBar,BOOL bManualOptimaze)
{
	CString StrPath = CPathFactory::GetUserInfoPath();
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

	bool32	bModified	=	false;
	while( NULL != pUserElement )
	{
		//先查找以前有这个的用户信息没
		//
		const char* StrUserName = pUserElement->Attribute(KStrElementAttriUserName);

		// 名称
		TCHAR TStrUserName[1024];
		memset(TStrUserName, 0, sizeof(TStrUserName));
		MultiCharCoding2Unicode(EMCCUtf8, StrUserName, strlen(StrUserName), TStrUserName, sizeof(TStrUserName) / sizeof(TCHAR));

		if ( strUserName.Compare(TStrUserName) == 0 )
		{
			//找到了，改变一下属性就可以了
			char	tmpBuf[1024];
			CString StrPwd = _T("");

			//
			CString	tmpStr;
			tmpStr.Format(_T("%d"), bShowSCToolBar);			
			Unicode2MultiCharCoding(EMCCUtf8, tmpStr, -1, tmpBuf, sizeof(tmpBuf));	//auto login
			pUserElement->SetAttribute(KStrElementAttriSCToolBar, tmpBuf);

			bModified	=	true;
		}
		//
		pUserElement	=	pUserElement->NextSiblingElement(KStrElementNameUser);
	}

	if ( !bModified )
	{
		return false;
	}

	bool bSave = myDocument.SaveFile();
	ASSERT( bSave );

	CCodeFile::EnCodeFile(StrUserInfoPath);

	return true;
}


LRESULT CDlgNewLogin::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: 在此添加专用代码和/或调用基类
	return CDialog::WindowProc(message, wParam, lParam);
}

LRESULT CDlgNewLogin::OnMsgMainFrameManualOptimize(WPARAM wParam, LPARAM lParam)
{
	CDlgManual Dlg;
	arrServer *paServersAftSort= (arrServer *)lParam;
	Dlg.SetInitialParams(*paServersAftSort);
	Dlg.DoModal();
	return 0;
}

// web第三方登录返回 qq,微信
LRESULT	CDlgNewLogin::OnWebThirdLoginRsp(WPARAM wParam, LPARAM lParam)
{
	CString* pStrType = (CString*)wParam;
	CString* pStrCode = (CString*)lParam;
	
	CGGTongApp *pApp  =  (CGGTongApp*) AfxGetApp();	
	CGGTongDoc *pDoc = pApp->m_pDocument;
	
	if ( NULL == pDoc )
	{
		return 0;
	}

	if(NULL != pStrType)
	{
		pDoc->m_StrLoginType =  *pStrType;
	}
	else
	{
		return 0;
	}

	if(NULL != pStrCode)
	{
		pDoc->m_StrOpenId =  *pStrCode;
	}
	else
	{
		return 0;
	}

	if (NULL != m_pDlgComment)
	{
		m_pDlgComment->EndDialog(IDOK);
	}

	//////////////////////////////////////////////////////////////////////////
	// 第三方登录

	SetPromptInfo(L"提示：认证中...");
	SetPromptInfo2(L"");
	COptimizeServer::Instance()->SetSortMode(!m_bManualOptimaze);
	m_bCancelAuth = false;

	// 获取服务器
	int32 iCurHost = m_indexServerSel;
	if (0 > iCurHost)
	{
		MessageBox(L"请选择服务器", L"提示");
		return 0;
	}

	// 如果不匹配, 默认取第一台
	if (iCurHost >= m_aHostInfo.GetSize())
	{
		iCurHost = 0;
		m_indexServerSel = 0;
	}

	T_HostInfo stHostInfo = m_aHostInfo[iCurHost];

	vector<UINT> vUiServerBackup(m_vUiServerBackup);
	if (!m_bGreenChannel)
	{
		vUiServerBackup.clear();
	}
	pDoc->m_pAutherManager->GetInterface()->ConnectServer(stHostInfo.StrHostAddress, vUiServerBackup);
	m_strSelService = stHostInfo.StrHostAddress;

	int iChannel = _ttoi(pDoc->m_StrLoginType);
	pDoc->m_pAutherManager->GetInterface()->ThirdAuthorize(pDoc->m_StrOpenId, CConfigInfo::Instance()->GetOrgKey(), iChannel);
	pDoc->m_pAbsCenterManager->SetLoginState(ELSAuthing);
	SetControlsState(pDoc->m_pAbsCenterManager->GetLoginState());

	return 0;
}

LRESULT	CDlgNewLogin::OnWebCloseDlg(WPARAM wParam, LPARAM lParam)
{
	if (NULL != m_pDlgComment)
	{
		m_pDlgComment->EndDialog(IDOK);
	}
	return 0;
}



void CDlgNewLogin::ShowThirdPartyLogin(int32 iID)
{
	CGGTongApp* pApp = (CGGTongApp*)AfxGetApp();
	CButtonWebConfig btnWeb = pApp->m_pConfigInfo->m_aThirdPartyLoginButtons[iID-ID_LOGIN_THIRDPARTY_QQ];
	if ( !btnWeb.m_strUrl.IsEmpty() )
	{
		bool32 bShowIE = FALSE;
		int32 iSize = pApp->m_pConfigInfo->m_aWndSize.GetSize();
		int32 iWidth = 1024, iHeight = 768;
		for (int32 i=0; i<iSize; i++)
		{
			CWndSize wnd = pApp->m_pConfigInfo->m_aWndSize[i];
			if (btnWeb.m_StrName == wnd.m_strID)
			{
				iWidth = wnd.m_iWidth;
				iHeight = wnd.m_iHeight;
				if (0==iWidth || 0== iHeight)
				{
					bShowIE = TRUE;
				}
				break;
			}
		}
		if (bShowIE)
		{
			ShellExecute(0, L"open", btnWeb.m_strUrl, NULL, NULL, SW_NORMAL);
		}
		else
		{
			if (m_pDlgComment != NULL)
			{
				m_pDlgComment->ShowDlgIEWithSize(btnWeb.m_StrName, btnWeb.m_strUrl, CRect(0, 0, iWidth, iHeight));
			}
		}
	}
}

bool32			CDlgNewLogin::ReqAuth()
{
	bool32 bReq = false;
	CGGTongDoc *pDoc = (CGGTongDoc*)AfxGetDocument();
	do 
	{
		if (NULL == pDoc )
		{
			break;
		}
		if(NULL == pDoc->m_pAbsCenterManager)
		{
			break;
		}

		ASSERT(pDoc->m_pAutherManager);
		ASSERT(pDoc->m_pAutherManager->GetInterface());
		if (NULL == pDoc->m_pAutherManager)
		{
			break;
		}
		if (NULL == pDoc->m_pAutherManager->GetInterface())
		{
			break;
		}			
		//	md5 密码32位加密
		int32 iPwdLength = m_StrPassword.GetLength();
		if (32 != iPwdLength)		// 不等于32md5加密， 等于32已经进行md5加密
		{
			string sPwd, sPwdMd5;
			UnicodeToUtf8(m_StrPassword, sPwd);
			MD5(sPwd, sPwdMd5);
			m_StrPassword = sPwdMd5.c_str();
			UpdateData(false);
		}
		//	设置viewdata中的登录状态
		pDoc->m_pAbsCenterManager->SetLoginState(ELSAuthing);
		//	重新验证下账户密码授权
		bReq = pDoc->m_pAutherManager->GetInterface()->Authorize(m_StrUserName, m_StrPassword, CConfigInfo::Instance()->GetOrgKey(), CConfigInfo::Instance()->GetVersionNo());
	} while (0);

	return bReq;
}
//	请求权限更新
bool32	CDlgNewLogin::ReqRight()
{
	bool32 bReq = false;
	CGGTongDoc *pDoc = (CGGTongDoc*)AfxGetDocument();
	do 
	{
		if (NULL == pDoc )
		{
			break;
		}
		if(NULL == pDoc->m_pAbsCenterManager)
		{
			break;
		}
		
		ASSERT(pDoc->m_pAutherManager);
		ASSERT(pDoc->m_pAutherManager->GetInterface());
		if (NULL == pDoc->m_pAutherManager)
		{
			break;
		}
		if (NULL == pDoc->m_pAutherManager->GetInterface())
		{
			break;
		}			
		// 请求权限列表
		bReq = pDoc->m_pAutherManager->GetInterface()->ReqQueryUserRight(CConfigInfo::Instance()->GetModule(), 2);
		// 请求权限提示
		bReq &= pDoc->m_pAutherManager->GetInterface()->ReqQueryRightTip(CConfigInfo::Instance()->GetModule());

	} while (0);

	return bReq;

}
// 认证成功开始请求配置信息(广告->文本配置->报价表底部市场->工具栏文件)
void CDlgNewLogin::ReqCommConfigInfo()
{
	SetPromptInfo(L"请求配置信息...");

	CGGTongApp *pApp  =  (CGGTongApp*) AfxGetApp();
	if (NULL == pApp || NULL == pApp->m_pDocument || NULL == pApp->m_pDocument->m_pAbsCenterManager)
	{
		ASSERT(0);
		return ;
	}
		
	CGGTongDoc *pDoc = pApp->m_pDocument;
	CAbsCenterManager *pAbsCenterManager = pDoc->m_pAbsCenterManager;		
	CString StrVer = pApp->m_pConfigInfo->m_StrToolBarVer;	// 工具栏文件版本号，没有可不填写		
	int32 iReqConfigCnt = 6;

	// 请求服务器市场相关信息
	pDoc->m_pAutherManager->GetInterface()->ReqQueryServerInfo(CConfigInfo::Instance()->GetAuthKey(),CConfigInfo::Instance()->GetOrgKey(),48, true,1);

	// 请求权限列表
	pDoc->m_pAutherManager->GetInterface()->ReqQueryUserRight(CConfigInfo::Instance()->GetModule(), 2);

	// 请求权限提示
	pDoc->m_pAutherManager->GetInterface()->ReqQueryRightTip(CConfigInfo::Instance()->GetModule());

	// 请求广告信息
	pDoc->m_pAutherManager->GetInterface()->ReqAdvInfo(CConfigInfo::Instance()->GetModule(), CConfigInfo::Instance()->GetOrgKey());

	// 请求文字导航
	pDoc->m_pAutherManager->GetInterface()->ReqQueryBanner();

	// 请求报价表底部市场
	pDoc->m_pAutherManager->GetInterface()->ReqReportTabList();


	// 非游客登录请求下载用户自选股数据
	if (!m_bDefaultUser)
	{
		pDoc->m_pAutherManager->GetInterface()->ReqQueryCusoptional(1);
		iReqConfigCnt++;
	}

	if(pApp->m_pConfigInfo->m_bXmlFileUpdate)
	{
		// 请求工具栏文件
		pDoc->m_pAutherManager->GetInterface()->ReqToolbarFile(StrVer);
		iReqConfigCnt++;
	}

	
	pAbsCenterManager->SetConfigRequstCount(iReqConfigCnt);	

}
void CDlgNewLogin::OnEnChangeEditUser()
{
	UpdateData(TRUE);
	int32 iUserIndex = 0;
	for ( ; iUserIndex < m_aUserInfo.GetSize() ; iUserIndex++)
	{
		if ( m_aUserInfo[iUserIndex].BeLastUser )
		{
			if(0 == m_StrUserName.CompareNoCase(m_aUserInfo[iUserIndex].StrUserName))
			{
				m_StrPassword = m_aUserInfo[iUserIndex].StrPassword;
				m_indexServerSel = m_aUserInfo[iUserIndex].IndexServer;
				m_bSavePwd = m_aUserInfo[iUserIndex].BeSavePassword;
				m_bAutoLogin  = m_aUserInfo[iUserIndex].IsAtutoLogin;
				m_bShowSCToolBar  = m_aUserInfo[iUserIndex].IsShowSCToolBar;
			}
			else
			{
				m_StrPassword.Empty();
			}
			UpdateData(false);
			break;
		}
	}
}

void CDlgNewLogin::OnNcLButtonDown(UINT nHitTest, CPoint point)
{
	ScreenToClient(&point);
	if( m_rcLeft.PtInRect(point) )
	{
		CGGTongApp* pApp = (CGGTongApp*)AfxGetApp();
		CString StrAdvsCode = pApp->m_pConfigInfo->m_StrLoginLeftCode;
		CString StrAdvsUrl = pApp->m_pConfigInfo->GetAdvsCenterUrlByCode(StrAdvsCode,EAUTLink);
		if(!StrAdvsUrl.IsEmpty())
			ShellExecute(0, L"open", StrAdvsUrl, NULL, NULL, SW_NORMAL);
	}
	
	CDialog::OnNcLButtonDown(nHitTest, point);
}

// md5加密
void CDlgNewLogin::MD5(std::string strSr, std::string &strResult)
{
	unsigned char encrypt[56];
	memset(encrypt, 0x00, 56);

	int iSize = strSr.size();
	if (56 < iSize)
	{
		return;
	}

	for (int k=0; k<iSize; k++)
	{
		encrypt[k] = strSr[k];
	}

	unsigned char decrypt[16];
	MD5_CTX md5;
	MD5Init(&md5);         		
	MD5Update(&md5,encrypt,strlen((char *)encrypt));
	MD5Final(&md5,decrypt);
	for(int k2=0; k2<16; k2++)
	{
		char buffer[3]={0};
		sprintf(buffer, "%02x", decrypt[k2]);
		strResult.append(buffer);
	}
}

