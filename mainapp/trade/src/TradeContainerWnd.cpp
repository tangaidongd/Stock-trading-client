#include "StdAfx.h"
#include "TradeContainerWnd.h"
#include <algorithm>
#include "MyDlgBar.h"
#include "coding.h"
#include "DlgTip.h"
#include "MerchManager.h"
#include <mmsystem.h>

#define TRADECONTAINERCLASSNAME		_T("TradeContainer")
#define IDC_BUTTON_TRADELOGIN_SELF	10000

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
// 
 namespace
 {
	static const short		KiBtnCloseID	= 100;
	static const short		KiBtnMaxID	= 101;
	static const short		KiBtnMinID	= 102;

	static const int		kiTimerIdDebugLoginTest1 = 101;
	static const int		kiTimerIdDebugLoginTestCount = 5;		// n 个timer来测试

	static const int		kiTimerIdHeartBeat = 110;			// 心跳包
	static const int		kiTimerPeriodHeartBeat = 10*1000;	//

	static const int		kiTimerIdTradeLoginTimeout = 111;			// 登录超时
	static const int		kiTimerPeriodTradeLoginTimeout = 45*1000;	//

	static const int		kiTimerPeriodDebugLoginTest = 1000;

	static const int		kiBtnWidth = 24;
	static const int		kiBtnSeparatorWidth = 2;

	static const char		k_szCelueName[] = "TradePoly";
	static const char		k_szMyName[] = "TradeWindow";

	void	DrawVerticalText(CDC &dc, const CString &Str, CRect rcDraw)
	{
		// 竖着书写文字
		CString StrTest(_T("标准宽度"));
		CSize sizeTest = dc.GetTextExtent(StrTest);
		int iTextHeight = sizeTest.cy;
		
		int iX = rcDraw.left;
		int iY = rcDraw.top;

		int iLen = Str.GetLength();
		for (int i = 0; i < iLen; i++)
		{
			CString strSingleChar(Str[i], 1);
			//dc.TextOut(iX, iY, strSingleChar);
			CRect rcSingle(iX, iY, rcDraw.right, iY + iTextHeight);
			dc.DrawText(strSingleChar, rcSingle, DT_SINGLELINE|DT_CENTER);
			
			//
			iY += iTextHeight ;
		}
	}
 }
// 
//////////////////////////////////////////////////////////////////////////
// iTradeBid		*GetCurrentTradeBid()
// {
// 	return NULL;//s_TradeBidCurrent;
// }

//////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CTradeContainerWnd, CRectButtonWndSimpleImpl);

BEGIN_MESSAGE_MAP(CTradeContainerWnd, CRectButtonWndSimpleImpl)
ON_WM_CREATE()
ON_WM_DESTROY()
ON_WM_LBUTTONDBLCLK()
ON_NOTIFY_REFLECT(NM_CLICK, OnBtnClick)
ON_WM_SETTINGCHANGE()
ON_WM_TIMER()
ON_COMMAND(IDC_BUTTON_TRADELOGIN_SELF, OnBtnTradeLogin)
ON_MESSAGE(WM_RESP_TRADE_NOTIFY, OnMsgRespTradeNotify)
ON_MESSAGE(WM_HIDETRADEWND, OnMsgHideTradeWnd)
END_MESSAGE_MAP()

CTradeContainerWnd::CTradeContainerWnd(E_TradeLoginType eTradeType)
{
	WNDCLASS	wndcls = {0};
	HINSTANCE	hInst = AfxGetInstanceHandle();
	wndcls.style			= CS_DBLCLKS;
	wndcls.lpfnWndProc		= ::DefWindowProc;
	wndcls.cbClsExtra		= wndcls.cbWndExtra = 0;
	wndcls.hInstance		= hInst;
	wndcls.hIcon			= NULL;
	wndcls.hCursor			= LoadCursor(NULL, IDC_ARROW);
	wndcls.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wndcls.lpszMenuName		= NULL;
	wndcls.lpszClassName	= TRADECONTAINERCLASSNAME;
	BOOL b = AfxRegisterClass(&wndcls);
	ASSERT( b );


	m_pBtnClose = m_pBtnMax = m_pBtnMin = NULL;

	m_pDlgLogin = new CDlgTradeLogin(eTradeType, this);
	m_pLoginNotify = NULL;

	m_pDlgTradeBidParent = new CDlgTradeBidContainer(this, eTradeType);

	memset(m_szAccountCur, 0, sizeof(m_szAccountCur));
 	m_bLoginSuc = false;
 	m_eLoginStatus = ETLS_NotLogin;

	m_lWaitTradeDTPProcess = false;
	m_bWndMaxMinStatus = TRUE;
	m_MaxSize.cy = 0;
	m_MaxSize.cx = 0;

	iRespCnt = 0;
	m_MaxRespCnt = 6;
	m_bRecvMsg = TRUE;
}

CTradeContainerWnd::~CTradeContainerWnd()
{
	m_tradeClient->Destroy();
	DEL(m_respTradeNotify);

	DEL(m_pBtnClose);
	DEL(m_pBtnMax);
	DEL(m_pBtnMin);
	DEL(m_pDlgLogin);
//	DEL(m_pDlgTradeBidParent);
	
	m_aCacheHoldDetail.clear();
	m_aCacheEntrust.clear();
	m_aCacheDeal.clear();
	m_aCacheCommInfo.clear();
	m_aCacheQuotation.clear();
	m_aCacheTraderID.clear();
}

int CTradeContainerWnd::Create( LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext /* = NULL */ )
{
	return CWnd::Create(TRADECONTAINERCLASSNAME, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
}

int CTradeContainerWnd::OnCreate( LPCREATESTRUCT lpCreateStruct )
{
	int iRet = CRectButtonWndSimpleImpl::OnCreate(lpCreateStruct);

	m_respTradeNotify = new CRespTradeNotify(this->m_hWnd);
	m_tradeClient = CreateTradeClient();
	m_tradeClient->SetNotifyer(m_respTradeNotify);

	ASSERT( NULL==m_pBtnClose );
	m_pBtnClose = new CWndRectButton(*this, this, KiBtnCloseID, CWndRectButton::Marlett_Close, _T("退出交易"));
	m_pBtnMax = new CWndRectButton(*this, this, KiBtnMaxID, CWndRectButton::Marlett_MAX, _T("最大化"));
	m_pBtnMin = new CWndRectButton(*this, this, KiBtnMinID, CWndRectButton::Marlett_Min, _T("最小化"));

	m_pBtnClose->SetDrawStyle(CWndRectButton::Draw_Style2);
	m_pBtnMax->SetDrawStyle(CWndRectButton::Draw_Style2);
	m_pBtnMin->SetDrawStyle(CWndRectButton::Draw_Style2);

	SetBtnColors();

 	m_pDlgLogin->Create(this);
 	m_pDlgLogin->ShowWindow(SW_HIDE);

 	m_pDlgTradeBidParent->m_pTradeLoginInfo = &m_stTradeLoginInfo;
	m_pDlgTradeBidParent->GetQueryDlg().SetTradeBid(this);
	m_pDlgTradeBidParent->GetTradeCloseDlg().SetTradeBid(this);
 	m_pDlgTradeBidParent->GetTradeOpenDlg().SetTradeBid(this);
 	m_pDlgTradeBidParent->GetTradeQuickOrderDlg().SetTradeBid(this);
//	m_pDlgTradeBidParent->GetChangePwdDlg().SetTradeBid(this);
	m_pDlgTradeBidParent->Create(this);
	m_pDlgTradeBidParent->ShowWindow(SW_HIDE);

	// 生成登录按钮
	m_BtnLogin.Create(L"交易登录", WS_CHILD | BS_DEFPUSHBUTTON, CRect(0, 0, 100, 30), this, IDC_BUTTON_TRADELOGIN_SELF);
	m_BtnLogin.ShowWindow(SW_HIDE);

	return iRet;
}

void CTradeContainerWnd::OnDestroy()
{
	m_pDlgTradeBidParent->GetQueryDlg().WndClose(3);

	if ( IsLogin() )
	{
		DoStopTrade();
	}

	CRectButtonWndSimpleImpl::OnDestroy();
}

void CTradeContainerWnd::GetAllBtns( WndRectButtonVector &aBtns )
{
	aBtns.clear();
	
	aBtns.push_back(m_pBtnClose);
	aBtns.push_back(m_pBtnMax);
	aBtns.push_back(m_pBtnMin);
}

void CTradeContainerWnd::OnDraw( CDC &dc )
{
	// 绘制背景
// 	CRect rc(0,0,0,0);
// 	GetClientRect(rc);
// 
// 	COLORREF clrWin = GetSysColor(COLOR_BTNFACE);
// 	dc.FillSolidRect(rc, clrWin);
// 
// 
// 	CRect rcSep(rc);
// 	rcSep.right = rc.right - kiBtnWidth;
// 	rcSep.left	= rcSep.right - kiBtnSeparatorWidth;
// 	//dc.DrawEdge(rcSep, EDGE_SUNKEN, BF_LEFT);
// 	dc.Draw3dRect(rcSep, RGB(170,170,170), RGB(250,250,250));
// 
// 	// 按钮绘制
// 	CRectButtonWndSimpleImpl::OnDraw(dc);
// 
// 	// 按钮下面绘制文字
// 	COLORREF clrText = GetSysColor(COLOR_WINDOWTEXT);
// 	CFont fontTitle;
// 	LOGFONT lg = {0};
// 	lg.lfHeight = -14;
// 	lg.lfCharSet = GB2312_CHARSET;
// 	lg.lfWeight = 700;
// 	_tcscpy(lg.lfFaceName, _T("宋体"));
// 	fontTitle.CreateFontIndirect(&lg);
// 	dc.SelectObject(&fontTitle);
// 	dc.SetBkMode(TRANSPARENT);
// 	dc.SetTextColor(clrText);
// 
// 	CRect rcTitle(rc);
// 	rcTitle.left = rcSep.right;
// 	WndRectButtonVector aBtns;
// 	GetAllBtns(aBtns);
// 	for (int32 i=aBtns.size()-1; i >= 0; i-- )
// 	{
// 		CRect rcBtn;
// 		aBtns[i]->GetBtnRect(rcBtn);
// 		rcTitle.top = max(rcTitle.top, rcBtn.bottom);
// 	}
// 	rcTitle.top += 4;
// 	CString StrTitle(_T("ＺＤＰ傻瓜交易系统"));
// 	DrawVerticalText(dc, StrTitle, rcTitle);
}

void CTradeContainerWnd::RecalcLayout( bool32 bNeedPaint /*= true*/ )
{
	const int iBtnWidth = kiBtnWidth;

	CRect rc(0,0,0,0);
	GetClientRect(rc);

	if ( rc.Width()>iBtnWidth )
	{
		// 其它子控件分配~~
		CRect rcOther(rc);

 		m_pDlgTradeBidParent->MoveWindow(rcOther);

// 		// 登录界面显示位置
// 		CRect rcDlgLogin(rc);
// 		rcDlgLogin.left = (rc.right-rc.left)/2;
// 		rcDlgLogin.top = rc.top+50;
// // 		rcDlgLogin.right -= iBtnWidth;
// // 		rcDlgLogin.right -= kiBtnSeparatorWidth;
// 		m_pDlgLogin->MoveWindow(rcDlgLogin);

		// 画登录按钮
		CRect rcBtnlogin(rc);
		rcBtnlogin.right = rc.left+200;
		rcBtnlogin.top = rc.top+50;
		rcBtnlogin.bottom = rcBtnlogin.top+30;

		m_BtnLogin.MoveWindow(rcBtnlogin);
	}


	if ( bNeedPaint )
	{
		Invalidate(TRUE);
	}
}

CMyContainerBar * CTradeContainerWnd::GetParentBar() const
{
	CWnd *pParent = GetParent();
	return DYNAMIC_DOWNCAST(CMyContainerBar, pParent);
}

BOOL CTradeContainerWnd::Maximize()
{
	CMyContainerBar *pParentBar = GetParentBar();
	if ( NULL!=pParentBar )
	{
		return pParentBar->Maximize();
	}
	
	ShowWindow(SW_MAXIMIZE);
	return TRUE;
}

BOOL CTradeContainerWnd::IsMaximized() const
{
	CMyContainerBar *pParentBar = GetParentBar();
	if ( NULL!=pParentBar )
	{
		return pParentBar->IsMaximized();
	}
	
	return IsZoomed();
}

BOOL CTradeContainerWnd::RestoreSize()
{
	CMyContainerBar *pParentBar = GetParentBar();
	if ( NULL!=pParentBar )
	{
		return pParentBar->RestoreSize();
	}
	
	ShowWindow(SW_RESTORE);
	return TRUE;
}

BOOL CTradeContainerWnd::Minimize()
{
	CMyContainerBar *pParentBar = GetParentBar();
	if ( NULL!=pParentBar )
	{
		return pParentBar->Minimize();
	}
	
	ShowWindow(SW_MINIMIZE);
	return TRUE;
}

BOOL CTradeContainerWnd::IsMinimized()
{
	CMyContainerBar *pParentBar = GetParentBar();
	if ( NULL!=pParentBar )
	{
		return pParentBar->IsMinimized();
	}
	
	return IsIconic() || (GetStyle()|WS_MINIMIZE) ;
}

void CTradeContainerWnd::DoMyToggleDock()
{
	CMyContainerBar *pParentBar = GetParentBar();
	if ( NULL!=pParentBar )
	{
		pParentBar->ToggleMyDock();
	}
}

void CTradeContainerWnd::OnLButtonDblClk( UINT nFlags, CPoint point )
{
	TOOLINFO stInfo = {0};
	int iToolHit = OnToolHitTest(point, &stInfo);
	if ( iToolHit == -1  || iToolHit == IDC_STATIC )
	{
		DoMyToggleDock();
		return;
	}
	
	CRectButtonWndSimpleImpl::OnLButtonDblClk(nFlags, point);
}

void CTradeContainerWnd::OnTradeClose()
{
	if ( IsLogin() )
	{
		CString StrQues = _T("是否退出交易？");
		if ( IDYES!=MessageBox(StrQues, _T("提示"), MB_ICONQUESTION|MB_YESNO) )
		{
			return;
		}

		if ( NULL!=m_pLoginNotify )
		{
			m_pLoginNotify->OnLoginStep(_T("交易登录."));
		}
	}
	// 交易关闭
	DoStopTrade();

	// 关闭后最小化
	Minimize();
}

void CTradeContainerWnd::OnTradeMax()
{
	if ( IsMaximized() )
	{
		// 按钮替换成最大化
		CString Str;
		Str += (TCHAR)(CWndRectButton::Marlett_MAX);
		m_pBtnMax->SetBtnText(Str);
		RestoreSize();
		// 显示最大化
	}
	else
	{
		// 按钮替换成还原
		CString Str;
		Str += (TCHAR)(CWndRectButton::Marlett_RESTORE);
		m_pBtnMax->SetBtnText(Str);
		Maximize();
		// 显示最大化
	}
}

void CTradeContainerWnd::OnTradeMin()
{
	if ( IsMinimized() )
	{
		RestoreSize();
	}
	else
	{
		// 显示最小化
		Minimize();
	}	
}

void CTradeContainerWnd::OnBtnClick( NMHDR *pHdr, LRESULT *pResult )
{
	// 按照以前的格式转发给wndParent
	if ( pResult != NULL && pHdr != NULL )
	{
		RECTBTN_NOTIFY_ITEM *pItem = (RECTBTN_NOTIFY_ITEM *)pHdr;
		*pResult = 1;

		switch ( pItem->uBtnId )
		{
		case KiBtnCloseID:
			OnTradeClose();
			break;
		case KiBtnMaxID:
			OnTradeMax();
			break;
		case KiBtnMinID:
			OnTradeMin();
			break;
		}
	}
}

void CTradeContainerWnd::SetBtnColors()
{
	COLORREF clrBk = GetSysColor(COLOR_BTNFACE);
	m_pBtnClose->SetColor(CWndRectButton::BTN_COLOR_Background, CWndRectButton::BTN_Background1, clrBk);
	m_pBtnClose->SetColor(CWndRectButton::BTN_COLOR_Background, CWndRectButton::BTN_Background2, clrBk);

	m_pBtnMax->SetColor(CWndRectButton::BTN_COLOR_Background, CWndRectButton::BTN_Background1, clrBk);
	m_pBtnMax->SetColor(CWndRectButton::BTN_COLOR_Background, CWndRectButton::BTN_Background2, clrBk);

	m_pBtnMin->SetColor(CWndRectButton::BTN_COLOR_Background, CWndRectButton::BTN_Background1, clrBk);
	m_pBtnMin->SetColor(CWndRectButton::BTN_COLOR_Background, CWndRectButton::BTN_Background2, clrBk);
}

void CTradeContainerWnd::OnSettingChange( UINT uFlags, LPCTSTR lpszSection )
{
	SetBtnColors();
	Invalidate(TRUE);
}

bool CTradeContainerWnd::Connect(const T_TradeLoginInfo &stLoginInfo, iTradeLoginNotify *pNotify)
{
	m_pLoginNotify = NULL;
	DisConnect();
	
	m_pLoginNotify = pNotify;
	m_stTradeLoginInfo = stLoginInfo;
	m_pDlgTradeBidParent->m_pTreeMenu->SetTreeStyle(m_stTradeLoginInfo.eTradeLoginType);
	
	if ( NULL==pNotify )
	{
		ASSERT( 0 );
		return false;
	}

	CProxyInfo ProxyInfo;
	bool32 bProxyInfo = false;
	if (0 != m_stTradeLoginInfo.m_proxyInfol.uiProxyType) // 有代理
	{
		ProxyInfo.m_eProxyType = (CProxyInfo::E_ProxyType)m_stTradeLoginInfo.m_proxyInfol.uiProxyType;
		ProxyInfo.m_StrProxyAddr = m_stTradeLoginInfo.m_proxyInfol.StrProxyAddress;
		ProxyInfo.m_iProxyPort = m_stTradeLoginInfo.m_proxyInfol.uiProxyPort;
		ProxyInfo.m_StrProxyUser = m_stTradeLoginInfo.m_proxyInfol.StrUserName;
		ProxyInfo.m_StrProxyPassword = m_stTradeLoginInfo.m_proxyInfol.StrUserPwd;
		bProxyInfo = true;
	}

	bool32 bRet;
	bRet = m_tradeClient->Connect(m_stTradeLoginInfo.StrServerIP, m_stTradeLoginInfo.iServerPort, bProxyInfo, ProxyInfo);
	if (!bRet)
	{
		CString StrErrMsg = _T("连接服务器失败");
		m_pLoginNotify->OnLoginError(StrErrMsg);
	 	m_pLoginNotify->OnUpdateValidCode();
		return false;
//		Login();
	}

	m_pDlgTradeBidParent->SetGridHeadColor();

	return true;
}

void CTradeContainerWnd::DisConnect(int iMode)
{
	m_tradeClient->DisConnect();
}

bool CTradeContainerWnd::Login()
{	
	if (NULL == m_pLoginNotify)
	{
		return false;
	}

	m_bRecvMsg = TRUE;
	m_eLoginStatus = ETLS_Logining;	// 正在登陆
	m_pLoginNotify->OnLoginStep(_T("开始登录..."));
	bool32 bRet;
	CClientReqLogIn reqLogIn;
	reqLogIn.account = m_stTradeLoginInfo.StrUser;
	reqLogIn.password = m_stTradeLoginInfo.StrPwd;
	bRet = m_tradeClient->ReqLogIn(reqLogIn);

	if (!bRet)
	{
		CString StrErrMsg = _T("用户登录失败");
		m_pLoginNotify->OnLoginError(StrErrMsg);
	 	m_pLoginNotify->OnUpdateValidCode();
		DisConnect(); // 断开连接
		return false;
	}

	return true;
}

void CTradeContainerWnd::LoginOK()
{
	m_bLoginSuc = false;
	const CClientRespLogIn &aLogIn = m_respTradeNotify->GetCacheLogIn();

	if (NULL == aLogIn.session_no || aLogIn.session_no <= 0)
	{
		CString StrErrMsg = _T("服务器未返回sessionID");
		m_pLoginNotify->OnLoginError(StrErrMsg);
	 	m_pLoginNotify->OnUpdateValidCode();
		return;
	}
	m_stTradeLoginInfo.StrLoginUser = m_stTradeLoginInfo.StrUser;
	m_stTradeLoginInfo.StrUser = aLogIn.account;
	m_stTradeLoginInfo.StrSessionID = aLogIn.session_no;
	m_stTradeLoginInfo.StrUserSessionID = aLogIn.user_session_id;
	m_stTradeLoginInfo.StrUserSessionInfo = aLogIn.user_session_info;
	m_stTradeLoginInfo.ThreadID = aLogIn.thread_id;
	m_stTradeLoginInfo.StrQuoteFlag = aLogIn.quote_flag;
	m_stTradeLoginInfo.StrMarketType = aLogIn.market_type;
			
	iRespCnt = 0;
	for (unsigned int i=0; i<m_aTradeBidNotify.size(); ++i)
	{
		iTradeBidNotify *pNotify = m_aTradeBidNotify[i];
		if ( pNotify && ETLS_NotLogin != m_eLoginStatus)
		{
			CString StrErrMsg = _T("");
		//	if (!(pNotify->OnInitQuery(StrErrMsg)))// 初始化查询
			if (!(pNotify->OnQueryUserInfo(StrErrMsg)))
			{
				m_pLoginNotify->OnLoginError(StrErrMsg);
 				m_pLoginNotify->OnUpdateValidCode();
							
				return;
			}
		}
	}
}

void CTradeContainerWnd::StopLogin(int iMode)
{	
	if ( NULL!=m_pLoginNotify&&iMode!=3 )
	{
		m_pLoginNotify->OnLoginUserStop();
	}
	
	DoStopTrade();

	// 要求停止登录
	m_pLoginNotify = NULL;
}

void CTradeContainerWnd::OnLoginDlgCancel()
{
	DoStopTrade();

	// 要求停止登录
	m_pLoginNotify = NULL;

	if (!IsLogin())	//肯定是没有登录的状态
	{
		m_pDlgLogin->ShowWindow(SW_HIDE);
		this->ShowWindow(SW_HIDE);
		
 		m_pDlgTradeBidParent->ShowWindow(SW_HIDE);
// 		m_pDlgTradeBidParent->SetFocus();
	}
	
	Minimize();
}

bool CTradeContainerWnd::IsLogin()
{
	return  m_bLoginSuc;
}

void CTradeContainerWnd::TryLogin()
{
	if ( IsLogin() )
	{
		// 显示交易界面
	}
	else
	{
		// 显示登录
		m_pDlgLogin->InitLogin(this);
	}

	ShowProperChild();
}

void CTradeContainerWnd::OnTimer( UINT nIDEvent )
{
	CWnd::OnTimer(nIDEvent);

// 	const int iLastId = kiTimerIdDebugLoginTest1+kiTimerIdDebugLoginTestCount-1;
// 	if ( nIDEvent>=kiTimerIdDebugLoginTest1 && nIDEvent<=iLastId )
// 	{
// 		KillTimer(nIDEvent);
// 	}
// 	else if ( kiTimerIdHeartBeat==nIDEvent )
// 	{
// 		
// 	}
// 	else if ( kiTimerIdTradeLoginTimeout==nIDEvent )
// 	{
//  		KillTimer(kiTimerIdTradeLoginTimeout);
// 		// 登录超时
// 		_AnsHead stHead;
// 		stHead.uStatus = -1;
// 		CString StrErr = _T("登录超时，请重新尝试.");
// 		SafeUnicode2MultiByte(CP_TRADEBID, StrErr, -1, stHead.chErrorMsg, sizeof(stHead.chErrorMsg));
// 		OnTradeFatalError(true, stHead);
// 	}
}

void CTradeContainerWnd::DoOnLoginSuc()
{
// 	KillTimer(kiTimerIdTradeLoginTimeout);
 	m_bLoginSuc = true;
// 
// 	DWORD dwTime = timeGetTime();
// 
// 	// 加载日志
// 	CString StrAcc;
// 	SafeMultiByte2Unicode(CP_TRADEBID, m_szAccountCur, -1, StrAcc);
// 	CTradeLogSave::Instance().SetCurrentTradeAccount(StrAcc);
// 	CTradeLogSave::Instance().Load();
// 
// 	if ( m_pLoginNotify )
// 	{
// 		m_pLoginNotify->OnLoginOK();
// 	}
// 
// 	SetLoginStatus(ETLS_Logined);
// 
// 	// 刷新品种信息
// // 	CTradeMerchDefaultVolCenter::Instance().RefreshConfig();
// // 	CTradeMerchDefaultVolCenter::Instance().Save();
// 	IncDTPNeedProcessFlag();
// 	T_ExportMT2DispatchThreadProcessorLParam *pParam = new T_ExportMT2DispatchThreadProcessorLParam;
// 	pParam->iEventId = ENT_DTPRefreshDefaultVolConfig;
// 	if ( !CExportMT2DispatchThread::Instance().Post2DispatchThread(this, (LPARAM)pParam) )
// 	{
// 		delete pParam;
// 		pParam = NULL;
// 		DecDTPNeedProcessFlag();
// 
// 		ASSERT( 0 );
// 	}
// 	
// 	ShowProperChild();
// 
// 	if ( NULL!=m_pDlgTradeBidParent )
// 	{
// 		m_pDlgTradeBidParent->GetBidDlg().FetchInitMerch();
// 	}
// 
// 	DoOnLog(k_szMyName, celueCore::ELTNormal, "登入交易系统.");
// 
// 	DWORD dwTime2 = timeGetTime();
// 	TRACE(_T("登录时长: %d\r\n"), dwTime2-dwTime);
}

void CTradeContainerWnd::CheckLoginSuc( int eRecvPackType )
{
// 	if ( ETLS_Logining == m_eLoginStatus )
// 	{
// 		DoOnLoginSuc();
// 	}
}

void CTradeContainerWnd::DoStopTrade()
{
 	m_bLoginSuc = false;
	m_eLoginStatus = ETLS_NotLogin;
// 	CString strName = m_stTradeLoginInfo.StrUser;
// 	CString strSessionID = m_stTradeLoginInfo.StrUserSessionID;
// 	CString strSessionInfo = m_stTradeLoginInfo.StrUserSessionInfo;
// 	
// 	bool32 bRet = m_tradeClient->ReqLogOut(strName, strSessionID, strSessionInfo); 
// 	
// 	// m_pLoginNotify = NULL;
// 
 	// 缓存信息清空
	m_aCacheHoldDetail.clear();
	m_aCacheEntrust.clear();
	m_aCacheDeal.clear();
	m_aCacheCommInfo.clear();
	m_aCacheQuotation.clear();
	m_aCacheTraderID.clear();

	// 	// 保存日志
	// 	DoOnLog(k_szMyName, celueCore::ELTNormal, "退出交易系统.\r\n");
// 	CTradeLogSave::Instance().Save();

 	SetLoginStatus(ETLS_NotLogin);

	//登出通知
	if (ETT_TRADE_FIRM == m_stTradeLoginInfo.eTradeLoginType)
	{
		OnTradeLogIn(-1);
	}
	else
	{
		OnSimulateTradeLogIn(-1);
	}
}

void CTradeContainerWnd::ShowProperChild()
{
	if ( IsLogin() )
	{
		// 显示交易界面
		m_pDlgLogin->ShowWindow(SW_HIDE); 
		
		m_pDlgTradeBidParent->ShowWindow(SW_SHOW);
		m_pDlgTradeBidParent->SetFocus();
	//	m_BtnLogin.ShowWindow(SW_HIDE);
	}
	else
	{
		// 显示登录
		m_pDlgLogin->ShowWindow(SW_SHOW);
		m_pDlgLogin->SetFocus();
	
		m_pDlgTradeBidParent->ShowWindow(SW_HIDE);
	//	Minimize();
	//	m_BtnLogin.ShowWindow(SW_SHOW);
	}
}

void CTradeContainerWnd::OnBtnTradeLogin()	// 登录操作
{
	TryLogin();
}

// iTradeBid
void CTradeContainerWnd::AddTradeBidNotify( iTradeBidNotify *pNotify )
{
	for ( int i=0; i<m_aTradeBidNotify.size(); ++i )
	{
		if ( m_aTradeBidNotify[i] == pNotify )
		{
			return;
		}
	}
	
	m_aTradeBidNotify.push_back(pNotify);
}

void CTradeContainerWnd::RemoveTradeBidNotify( iTradeBidNotify *pNotify )
{
	for ( int i=0; i<m_aTradeBidNotify.size(); ++i )
	{
		if ( m_aTradeBidNotify[i] == pNotify )
		{
			m_aTradeBidNotify.erase( m_aTradeBidNotify.begin()+i );
			return;
		}
	}
}

bool32 CTradeContainerWnd::ReqQueryHold(const CClientReqQueryHold& stReq, CString &strErrMsg)
{
	// 获取持仓明细信息
	bool32 bRet;
	bRet = m_tradeClient->ReqQueryHold(stReq);

	if (!bRet)
	{
		strErrMsg.Format(_T("查询持仓明细失败"));
		if (ETLS_Logined != m_eLoginStatus)	// 登录中
		{
			if (NULL != m_pLoginNotify)
			{
				m_pLoginNotify->OnLoginError(strErrMsg);
			}
		}

		return bRet;
	}

	return bRet;
}

bool32 CTradeContainerWnd::ReqQueryHoldSummary(const CClientReqQueryHoldTotal& stReq, CString &strErrMsg)
{
	bool32 bRet;
	bRet = m_tradeClient->ReqQueryHoldTotal(stReq);
	if (!bRet)
	{
		strErrMsg.Format(_T("查询持仓汇总失败"));
		if (ETLS_Logined != m_eLoginStatus)
		{
			if (NULL != m_pLoginNotify)
			{
				m_pLoginNotify->OnLoginError(strErrMsg);
			}
		}
		return bRet;
	}
	return bRet;
}

bool32 CTradeContainerWnd::ReqQueryEntrust(const CClientReqQueryEntrust& stReq, CString &strErrMsg)
{
	// 获取委托信息
	bool32 bRet;
	bRet = m_tradeClient->ReqQueryEntrust(stReq);

	if (!bRet)
	{
		strErrMsg.Format(_T("查询委托失败"));
		if (ETLS_Logined != m_eLoginStatus)	// 登录中
		{
			if (NULL != m_pLoginNotify)
			{
				m_pLoginNotify->OnLoginError(strErrMsg);
			}
		}
		return bRet;
	}
	
	return bRet;
}

bool32 CTradeContainerWnd::ReqQueryCancelEntrust(const CClientReqQueryCancelEntrust& stReq, CString &strErrMsg)
{
	bool32 bRet;
	bRet = m_tradeClient->ReqQueryCancelEntrust(stReq);

	if (!bRet)
	{
		strErrMsg.Format(_T("查询委托失败"));
		if (ETLS_Logined != m_eLoginStatus)	// 登录中
		{
			if (NULL != m_pLoginNotify)
			{
				m_pLoginNotify->OnLoginError(strErrMsg);
			}
		}
		return bRet;
	}

	return bRet;
}

bool32 CTradeContainerWnd::ReqQueryDeal(const CClientReqQueryDeal& stReq, CString &strErrMsg)
{
	// 获取成交信息
	bool32 bRet;
	bRet = m_tradeClient->ReqQueryDeal(stReq);

	if (!bRet)
	{
		strErrMsg.Format(_T("查询平仓单失败"));
		return bRet;
	}

	return bRet;
}

bool32 CTradeContainerWnd::ReqQueryTraderID(const CClientQueryTraderID& stReq, CString &strErrMsg)
{
	bool32 bRet;
	bRet = m_tradeClient->ReqQueryTraderID(stReq);

	if (!bRet)
	{
		strErrMsg.Format(_T("查询对方交易员失败"));
		if (ETLS_Logined != m_eLoginStatus)	// 登录中
		{
			if (NULL != m_pLoginNotify)
			{
				m_pLoginNotify->OnLoginError(strErrMsg);
			}
		}

		return bRet;
	}

	return bRet;
}

bool32 CTradeContainerWnd::ReqMerchInfo(const CClientReqMerchInfo& stReq, CString &strErrMsg)
{
	// 获取商品信息
	bool32 bRet;
	bRet = m_tradeClient->ReqMerchInfo(stReq);
	
	if ( !bRet)
	{
		strErrMsg.Format(_T("查询商品信息失败"));
		if (ETLS_Logined != m_eLoginStatus)	// 登录中
		{
			if (NULL != m_pLoginNotify)
			{
				m_pLoginNotify->OnLoginError(strErrMsg);
			}
		}

		return bRet;
	}

	if (ETLS_Logined != m_eLoginStatus)	// 登录中
	{
		if (NULL != m_pLoginNotify)
		{
			m_pLoginNotify->OnLoginStep(_T("正在加载商品信息......"));
		}
	}
	
	return bRet;
}

bool32 CTradeContainerWnd::ReqUserInfo(const CClientReqUserInfo& stReq, CString &strErrMsg)
{
	// 获取账户信息信息
	bool32 bRet;
	bRet = m_tradeClient->ReqUserInfo(stReq);
	
	if ( !bRet)
	{
		strErrMsg.Format(_T("查询账户信息失败"));	
		if (ETLS_Logined != m_eLoginStatus)	// 登录中
		{
			if (NULL != m_pLoginNotify)
			{
				m_pLoginNotify->OnLoginError(strErrMsg);
			}
		}
		return bRet;
	}

	if (ETLS_Logined != m_eLoginStatus)	// 登录中
	{
		if (NULL != m_pLoginNotify)
		{
			m_pLoginNotify->OnLoginStep(_T("正在加载账户信息......"));
		}
	}
	
	return bRet;
}

// 委托买卖
bool32 CTradeContainerWnd::ReqEntrust(const CClientReqEntrust& stReq, CString &strTipMsg)
{
	bool32 bRet;
	bRet = m_tradeClient->ReqEntrust(stReq);

	if ( !bRet)
	{
		strTipMsg.Format(_T("操作失败!"));
	}
	else
	{
		strTipMsg.Format(_T("操作成功!"));
	}
	
	return bRet;
}

// 请求撤销委托单
bool32 CTradeContainerWnd::ReqCancelEntrust(const CClientReqCancelEntrust& stReq, CString &strTipMsg)
{
	bool32 bRet;
	bRet = m_tradeClient->ReqCancelEntrust(stReq);

	if ( !bRet)
	{
		strTipMsg.Format(_T("操作失败!"));		
	}
	else
	{
		strTipMsg.Format(_T("操作成功!"));
	}
	
	return bRet;
}

// 撤止损止盈单
bool32 CTradeContainerWnd::ReqCancelStopLP(const CClientReqCancelStopLP& stReq, CString &strTipMsg)
{
	bool32 bRet;
	bRet = m_tradeClient->ReqCancelStopLP(stReq);

	if ( !bRet)
	{
		strTipMsg.Format(_T("操作失败!"));		
	}
	else
	{
		strTipMsg.Format(_T("操作成功!"));
	}
	
	return bRet;
}

// 设置止盈止损
bool32 CTradeContainerWnd::ReqSetStopLP(const CClientReqSetStopLP& stReq, CString &strTipMsg)
{
	bool32 bRet;
	bRet = m_tradeClient->ReqSetStopLP(stReq);

	if ( !bRet)
	{
		strTipMsg.Format(_T("操作失败!"));		
	}
	else
	{
		strTipMsg.Format(_T("操作成功!"));
	}
	
	return bRet;
}

// 请求修改密码
bool32 CTradeContainerWnd::ReqModifyPwd(const CClientReqModifyPwd& stReq, CString &strTipMsg)
{
	bool32 bRet;
	bRet = m_tradeClient->ReqModifyPwd(stReq);
	
	if ( !bRet)
	{
		strTipMsg.Format(_T("操作失败!"));		
	}
	else
	{
		strTipMsg.Format(_T("操作成功!"));
	}
	
	return bRet;
}

int CTradeContainerWnd::HeartBeating(CString &strTipMsg)
{	
	return 0;
// 	bool bSuc = false;
// 	
// 	char *pMsg = NULL;
// 	AuthoPlugin	autho;
// 
// 	string strUserID;
// 	string strSessionID;
// 	UnicodeToUtf8(m_stTradeLoginInfo.StrUser, strUserID);
// 	UnicodeToUtf8(m_stTradeLoginInfo.StrSessionID, strSessionID);
// 
// 	int iRet = autho.SendHeartCon(m_stTradeLoginInfo.serverobj,(char*)strUserID.c_str(),(char*)strSessionID.c_str(),(char*)KStrElementAttriVesion,&pMsg);
// 	if ( 0!=iRet)
// 	{
// 		wstring wstr;
// 		if (NULL!=pMsg)
// 		{
// 			Gbk32ToUnicode(pMsg,wstr);
// 			strTipMsg.Format(_T("%s"),wstr.c_str());
// 		}
// 		else
// 		{
// 			strTipMsg.Format(_T("操作失败!"));		
// 		}
// 		
// 		bSuc = false;
// 	}
// 	else
// 	{
// 		strTipMsg.Format(_T("操作成功!"));
// 		bSuc = true;
// 	}
// 	
// 	// 释放内存
// 	autho.Release(pMsg);
// 	
// 	return iRet;	
}

// 界面大小化
void CTradeContainerWnd::WndMaxMin()
{
	const int KLenTitleTop = 35;	// 缩下去的高度
	
	CMyContainerBar *pParentBar = GetParentBar();
	if (NULL==pParentBar)
	{
		return;
	}

	CSize size = pParentBar->GetSizeDefault();

	if (m_bWndMaxMinStatus)//当前是最大状态(点击了缩小按钮)
	{
		m_MaxSize = pParentBar->GetCurrentSize();
		m_bWndMaxMinStatus = FALSE;
		size.cy = KLenTitleTop;
		pParentBar->SetFixedHeight(true,size.cy);	//设定固定高度
		pParentBar->SetNewSize(size);
	}
	else
	{
		m_bWndMaxMinStatus = TRUE;
		pParentBar->SetFixedHeight(false,m_MaxSize.cy);	//不设定固定高度
		pParentBar->SetNewSize(m_MaxSize);
	}
}

// 退出交易
void CTradeContainerWnd::WndClose(int iMode)
{
	if (2==iMode)//向柜台发送注销请求
	{
		CString strName = m_stTradeLoginInfo.StrUser;
		CString strSessionID = m_stTradeLoginInfo.StrUserSessionID;
		CString strSessionInfo = m_stTradeLoginInfo.StrUserSessionInfo;

		m_tradeClient->ReqLogOut(strName, strSessionID, strSessionInfo); //不管注销成功或失败，都退出程序
	}

	if ( IsLogin()&&iMode!=3 )
	{		
		if ( NULL!=m_pLoginNotify )
		{
			m_pLoginNotify->OnLoginStep(_T("交易登录"));
		}
	}
	// 关闭交易
	StopLogin(iMode);//pTrade();
	
	// 关闭后最小化
	if (0==iMode)//重新登录
	{
		TryLogin();
	}
// 	else//注销
// 	{
// 		Minimize();
// 	}
	Minimize();
}

// 查行情
bool32 CTradeContainerWnd::ReqQute(const CClientReqQuote& stReq, CString &strErrMsg)
{
	bool32 bRet;
	bRet = m_tradeClient->ReqQute(stReq);
	
	if ( !bRet)
	{
		strErrMsg.Format(_T("查询行情信息失败"));
		return bRet;
	}
	
	return bRet;
}

E_TradeLoginStatus CTradeContainerWnd::GetTradeLoginStatus()
{
	return (E_TradeLoginStatus)m_eLoginStatus;
}

void CTradeContainerWnd::SetLoginStatus( int eStatus )
{
	for ( int i=0; i<m_aTradeBidNotify.size(); ++i )
	{
		iTradeBidNotify *pNotify = m_aTradeBidNotify[i];
		if ( pNotify )
		{
			pNotify->OnLoginStatusChanged(eStatus,eStatus);
		}
	}
}

void CTradeContainerWnd::DoOnQueryHoldDetailResponse()
{
	m_aCacheHoldDetail.clear();
	if (ETLS_NotLogin == m_eLoginStatus)
	{
		return;
	}
	m_aCacheHoldDetail =  m_respTradeNotify->GetCacheHoldDetail();

	for ( int i=0; i<m_aTradeBidNotify.size(); ++i )
	{
		iTradeBidNotify *pNotify = m_aTradeBidNotify[i];
		if ( pNotify )
		{
			pNotify->OnQueryHoldDetailResponse();// 填充持仓明细表格
		}
	}
	if (m_MaxRespCnt > iRespCnt)
	{
		iRespCnt++;
		if (ETLS_Logined != m_eLoginStatus)	// 登录中
		{
			if (NULL != m_pLoginNotify)
			{
				m_pLoginNotify->OnLoginStep(_T("正在加载持仓汇总......"));
			}
		}
	}
	else if ((m_MaxRespCnt == iRespCnt) && (ETLS_NotLogin != m_eLoginStatus))
	{
		iRespCnt = m_MaxRespCnt+1;
		m_eLoginStatus = ETLS_Logined;	
		SetLoginStatus(ETLS_Logined);
	
		if (ETT_TRADE_FIRM == m_stTradeLoginInfo.eTradeLoginType)
		{
			OnTradeLogIn(1);
		}
		else
		{
			OnSimulateTradeLogIn(1, m_stTradeLoginInfo.StrQuoteFlag);
		}

		m_bLoginSuc = true;
		ShowProperChild();	// 通知显示交易界面，隐藏登陆界面
		
		//通知交易界面显示
		if (ETT_TRADE_FIRM == m_stTradeLoginInfo.eTradeLoginType)
		{
			GetParent()->PostMessage(WM_COMMAND, UM_FIRM_TRADE_SHOW);
		}
		else
		{
			GetParent()->PostMessage(WM_COMMAND, UM_SIMULATE_TRADE_SHOW);
		}
	}
}

void CTradeContainerWnd::DoOnQueryHoldSummaryResponse()
{
	if (ETLS_NotLogin == m_eLoginStatus)
		return;
	m_aCacheHoldSummary.clear();
	m_aCacheHoldSummary = m_respTradeNotify->GetCacheHoldSummary();

	for ( int i=0; i<m_aTradeBidNotify.size(); ++i )
	{
		iTradeBidNotify *pNotify = m_aTradeBidNotify[i];
		if ( pNotify )
		{
			pNotify->OnQueryHoldSummaryResponse();// 填充持仓汇总表格
		}
	}

	if (m_MaxRespCnt > iRespCnt)
	{
		iRespCnt++;
		if (ETLS_Logined != m_eLoginStatus)	// 登录中
		{
			if (NULL != m_pLoginNotify)
			{
				m_pLoginNotify->OnLoginStep(_T("正在加载指价委托单......"));
			}
		}
	}
	else if ((m_MaxRespCnt == iRespCnt) && (ETLS_NotLogin != m_eLoginStatus))
	{
		iRespCnt = m_MaxRespCnt+1;
		m_eLoginStatus = ETLS_Logined;
		SetLoginStatus(ETLS_Logined);

		if (ETT_TRADE_FIRM == m_stTradeLoginInfo.eTradeLoginType)
		{
			OnTradeLogIn(1);
		}
		else
		{
			OnSimulateTradeLogIn(1, m_stTradeLoginInfo.StrQuoteFlag);
		}

		m_bLoginSuc = true;
		ShowProperChild();	// 通知显示交易界面，隐藏登陆界面

		//通知交易界面显示
		if (ETT_TRADE_FIRM == m_stTradeLoginInfo.eTradeLoginType)
		{
			GetParent()->PostMessage(WM_COMMAND, UM_FIRM_TRADE_SHOW);
		}
		else
		{
			GetParent()->PostMessage(WM_COMMAND, UM_SIMULATE_TRADE_SHOW);
		}
	}
}

void CTradeContainerWnd::DoOnQueryEntrustResponse()
{
	m_aCacheEntrust.clear();
	if (ETLS_NotLogin == m_eLoginStatus)
	{
		return;
	}
	m_aCacheEntrust = m_respTradeNotify->GetCacheEntrust();
	
	for ( int i=0; i<m_aTradeBidNotify.size(); ++i )
	{
		iTradeBidNotify *pNotify = m_aTradeBidNotify[i];
		if ( pNotify )
		{
			//pNotify->OnQueryEntrustResponse();
			pNotify->OnQueryLimitEntrustResponse();
		}
	}
	if (m_MaxRespCnt > iRespCnt)
	{
		iRespCnt++;
		if (ETLS_Logined != m_eLoginStatus)	// 登录中
		{
			if (NULL != m_pLoginNotify)
			{
				m_pLoginNotify->OnLoginStep(_T("正在加载平仓单......"));
			}
		}
	}
	else if ((m_MaxRespCnt == iRespCnt) && (ETLS_NotLogin != m_eLoginStatus))
	{
		iRespCnt = m_MaxRespCnt+1;
		m_eLoginStatus = ETLS_Logined;	
		SetLoginStatus(ETLS_Logined);
		
		if (ETT_TRADE_FIRM == m_stTradeLoginInfo.eTradeLoginType)
		{
			OnTradeLogIn(1);
		}
		else
		{
			OnSimulateTradeLogIn(1, m_stTradeLoginInfo.StrQuoteFlag);
		}

		m_bLoginSuc = true;
		ShowProperChild();	// 通知显示交易界面，隐藏登陆界面
		
		//通知交易界面显示
		if (ETT_TRADE_FIRM == m_stTradeLoginInfo.eTradeLoginType)
		{
			GetParent()->PostMessage(WM_COMMAND, UM_FIRM_TRADE_SHOW);
		}
		else
		{
			GetParent()->PostMessage(WM_COMMAND, UM_SIMULATE_TRADE_SHOW);
		}
	}
}

void CTradeContainerWnd::DoOnQueryDealResponse()
{
	m_aCacheDeal.clear();
	if (ETLS_NotLogin == m_eLoginStatus)
	{
		return;
	}
	m_aCacheDeal = m_respTradeNotify->GetCacheDeal();
	
	for ( int i=0; i<m_aTradeBidNotify.size(); ++i )
	{
		iTradeBidNotify *pNotify = m_aTradeBidNotify[i];
		if ( pNotify )
		{
			pNotify->OnQueryDealResponse();
		}
	}
	if (m_MaxRespCnt > iRespCnt)
	{
		iRespCnt++;
	}
	else if ((m_MaxRespCnt == iRespCnt) && (ETLS_NotLogin != m_eLoginStatus))
	{
		iRespCnt = m_MaxRespCnt+1;
		m_eLoginStatus = ETLS_Logined;	
		SetLoginStatus(ETLS_Logined);
		
		if (ETT_TRADE_FIRM == m_stTradeLoginInfo.eTradeLoginType)
		{
			OnTradeLogIn(1);
		}
		else
		{
			OnSimulateTradeLogIn(1, m_stTradeLoginInfo.StrQuoteFlag);
		}

		m_bLoginSuc = true;
		ShowProperChild();	// 通知显示交易界面，隐藏登陆界面
		
		//通知交易界面显示
		if (ETT_TRADE_FIRM == m_stTradeLoginInfo.eTradeLoginType)
		{
			GetParent()->PostMessage(WM_COMMAND, UM_FIRM_TRADE_SHOW);
		}
		else
		{
			GetParent()->PostMessage(WM_COMMAND, UM_SIMULATE_TRADE_SHOW);
		}
	}
}

void CTradeContainerWnd::DoOnQueryCommInfoResponse()
{
	m_aCacheCommInfo.clear();
	if (ETLS_NotLogin == m_eLoginStatus)
	{
		return;
	}
	m_aCacheCommInfo = m_respTradeNotify->GetCacheCommInfo();
	
	for ( int i=0; i<m_aTradeBidNotify.size(); ++i )
	{
		iTradeBidNotify *pNotify = m_aTradeBidNotify[i];
		if ( pNotify )
		{
			pNotify->OnQueryCommInfoResponse();
			pNotify->OnInitCommInfo();
		}
	}

	if (m_MaxRespCnt > iRespCnt)
	{
		iRespCnt++;
		if (ETLS_Logined != m_eLoginStatus)	// 登录中
		{
			if (NULL != m_pLoginNotify)
			{
				m_pLoginNotify->OnLoginStep(_T("正在加载持仓明细......"));
			}
		}
	}
	else if ((m_MaxRespCnt == iRespCnt) && (ETLS_NotLogin != m_eLoginStatus))
	{
		iRespCnt = m_MaxRespCnt+1;
		m_eLoginStatus = ETLS_Logined;	
		SetLoginStatus(ETLS_Logined);

		if (ETT_TRADE_FIRM == m_stTradeLoginInfo.eTradeLoginType)
		{
			OnTradeLogIn(1);
		}
		else
		{
			OnSimulateTradeLogIn(1, m_stTradeLoginInfo.StrQuoteFlag);
		}

		m_bLoginSuc = true;
		ShowProperChild();	// 通知显示交易界面，隐藏登陆界面
		
		//通知交易界面显示
		if (ETT_TRADE_FIRM == m_stTradeLoginInfo.eTradeLoginType)
		{
			GetParent()->PostMessage(WM_COMMAND, UM_FIRM_TRADE_SHOW);
		}
		else
		{
			GetParent()->PostMessage(WM_COMMAND, UM_SIMULATE_TRADE_SHOW);
		}
	}
}

void CTradeContainerWnd::DoOnQueryTraderIDResponse()
{
	m_aCacheTraderID.clear();
	if (ETLS_NotLogin == m_eLoginStatus)
	{
		return;
	}
	m_aCacheTraderID = m_respTradeNotify->GetCacheTraderID();

	for ( int i=0; i<m_aTradeBidNotify.size(); ++i )
	{
		iTradeBidNotify *pNotify = m_aTradeBidNotify[i];
		if ( pNotify )
		{
			pNotify->OnQueryTraderIDResponse();
		}
	}

	if (m_MaxRespCnt > iRespCnt)
	{
		iRespCnt++;
		if (ETLS_Logined != m_eLoginStatus)	// 登录中
		{
			if (NULL != m_pLoginNotify)
			{
				m_pLoginNotify->OnLoginStep(_T("正在加载对方交易员ID......"));
			}
		}
	}
	else if ((m_MaxRespCnt == iRespCnt) && (ETLS_NotLogin != m_eLoginStatus))
	{
		iRespCnt = m_MaxRespCnt+1;
		m_eLoginStatus = ETLS_Logined;	
		SetLoginStatus(ETLS_Logined);

		OnTradeLogIn(1);

		m_bLoginSuc = true;
		ShowProperChild();	// 通知显示交易界面，隐藏登陆界面

		//通知交易界面显示
		if (ETT_TRADE_FIRM == m_stTradeLoginInfo.eTradeLoginType)
		{
			GetParent()->PostMessage(WM_COMMAND, UM_FIRM_TRADE_SHOW);
		}
		else
		{
			GetParent()->PostMessage(WM_COMMAND, UM_SIMULATE_TRADE_SHOW);
		}
	}
}

void CTradeContainerWnd::DoOnQueryUserInfoResponse()
{
	if (ETLS_NotLogin == m_eLoginStatus)
	{
		return;
	}
	m_stCacheUserInfo = m_respTradeNotify->GetCacheUserInfo();
	
	for ( int i=0; i<m_aTradeBidNotify.size(); ++i )
	{
		iTradeBidNotify *pNotify = m_aTradeBidNotify[i];
		if ( pNotify )
		{
			pNotify->OnQueryUserInfoResponse();
		}
	}

	if (0 == iRespCnt)
	{
		for ( int i=0; i<m_aTradeBidNotify.size(); ++i )
		{
			iTradeBidNotify *pNotify = m_aTradeBidNotify[i];
			if ( pNotify && (ETLS_NotLogin != m_eLoginStatus))
			{
				CString StrErrMsg = _T("");
				if (!(pNotify->OnInitQuery(StrErrMsg)))// 初始化查询
				{
					m_pLoginNotify->OnLoginError(StrErrMsg);
				 	m_pLoginNotify->OnUpdateValidCode();
					
					return;
				}
			}
		}
		iRespCnt++;
	}
}

void CTradeContainerWnd::DoOnQueryCodeResponse()
{
	m_stCacheCode = m_respTradeNotify->GetCacheCode();
}

void CTradeContainerWnd::SetQuoteValue()
{
	for ( int i=0; i<m_aTradeBidNotify.size(); ++i )
	{
		iTradeBidNotify *pNotify = m_aTradeBidNotify[i];
		if ( pNotify )
		{
			pNotify->OnQueryQuotationResponse();
		}
	}
}

void CTradeContainerWnd::DoOnQueryQuotationResponse()
{
	m_aCacheQuotation.clear();
	m_aCacheQuotation = m_respTradeNotify->GetCacheQuotation();
	CClientRespQuote pAns;
	pAns.m_aQuote = m_aCacheQuotation;
	
	for ( int i=0; i<m_aTradeBidNotify.size(); ++i )
	{
		iTradeBidNotify *pNotify = m_aTradeBidNotify[i];
		if ( pNotify )
		{
			pNotify->OnQueryQuotationResponse();
		}
	}
	
	if (ETT_TRADE_FIRM == m_stTradeLoginInfo.eTradeLoginType)
	{
		//向行情推送交易行情数据
		OnTradeQuotationPush(&pAns,m_aCacheQuotation.size());
	}
}

void CTradeContainerWnd::DoOnErrorResponse()
{
	m_stCacheError = m_respTradeNotify->GetCacheError();
	if (ETLS_Logining == m_eLoginStatus)  // 正在登录
	{
		m_eLoginStatus = ETLS_NotLogin;
		m_pLoginNotify->OnLoginError(m_stCacheError.error_info);
		m_pLoginNotify->OnUpdateValidCode();
		m_stCacheError.error_no = 0;
		m_stCacheError.error_info = L"";
		return;
	}

	if (-1 == m_stCacheError.error_no) //帐号在其它地方登录
	{
		for ( int i=0; i<m_aTradeBidNotify.size(); ++i )
		{
			iTradeBidNotify *pNotify = m_aTradeBidNotify[i];
			if ( pNotify )
			{
				pNotify->OnDisConnectResponse();
			}
		}
		m_stCacheError.error_no = 0;
		m_stCacheError.error_info = L"";
		return;
	}
	
	if (L"" != m_stCacheError.error_info)
	{
		m_bRecvMsg = FALSE;
		CDlgTip dlg;
		dlg.m_strTipMsg = m_stCacheError.error_info;
		dlg.m_eTipType = ETT_ERR;
		dlg.m_pCenterWnd = this;	// 设置要居中到的窗口指针		
		dlg.DoModal();

		m_pDlgTradeBidParent->PostMessage(WM_CLOSETRADE,0,0);	//通知隐藏其他窗口

		WndClose(2);  // 出错重新登录
		TryLogin();
	}
}

void CTradeContainerWnd::DoOnDisConnectResponse()
{
	if (ETLS_Logining == m_eLoginStatus)// 正在登录
	{
		m_eLoginStatus = ETLS_NotLogin;
		m_pLoginNotify->OnLoginError(_T("与服务器连接断开"));
		return;
	}

	for ( int i=0; i<m_aTradeBidNotify.size(); ++i )
	{
		iTradeBidNotify *pNotify = m_aTradeBidNotify[i];
		if ( pNotify )
		{
			pNotify->OnDisConnectResponse();
		}
	}
}

void CTradeContainerWnd::DoOnReqEntrustResponse()
{
	bool32 bShow = true;
	if (0 < m_pDlgTradeBidParent->GetQueryDlg().m_iReqEntrustCnt)
	{
		m_pDlgTradeBidParent->GetQueryDlg().m_iReqEntrustCnt--;
		bShow = false;
	}

	m_stCacheReqEntrust = m_respTradeNotify->GetCacheReqEntrust();
	for ( int i=0; i<m_aTradeBidNotify.size(); ++i )
	{
		iTradeBidNotify *pNotify = m_aTradeBidNotify[i];
		if ( pNotify )
		{
			pNotify->OnReqEntrustResponse(bShow);
		}
	}
}

void CTradeContainerWnd::DoOnReqCancelEntrustResponse()
{
	m_stCacheReqCancelEntrust = m_respTradeNotify->GetCacheReqCancelEntrust();
	for ( int i=0; i<m_aTradeBidNotify.size(); ++i )
	{
		iTradeBidNotify *pNotify = m_aTradeBidNotify[i];
		if ( pNotify )
		{
			pNotify->OnReqCancelEntrustResponse();
		}
	}
}

void CTradeContainerWnd::DoOnReqModifyPwdResponse()
{
	m_stCacheReqModifyPwd = m_respTradeNotify->GetCacheReqModifyPwd();
	for ( int i=0; i<m_aTradeBidNotify.size(); ++i )
	{
		iTradeBidNotify *pNotify = m_aTradeBidNotify[i];
		if ( pNotify )
		{
			pNotify->OnReqModifyPwdResponse();
		}
	}
}

void CTradeContainerWnd::DoOnReqSetStopLPResponse()
{
	m_stCacheReqSetStopLP = m_respTradeNotify->GetCacheReqSetStopLP();
	for ( int i=0; i<m_aTradeBidNotify.size(); ++i )
	{
		iTradeBidNotify *pNotify = m_aTradeBidNotify[i];
		if ( pNotify )
		{
			pNotify->OnReqSetStopLPResponse();
		}
	}
}

void CTradeContainerWnd::DoOnReqCancelStopLPResponse()
{
	m_stCacheReqCancelStopLP = m_respTradeNotify->GetCacheReqCancelStopLP();
	for ( int i=0; i<m_aTradeBidNotify.size(); ++i )
	{
		iTradeBidNotify *pNotify = m_aTradeBidNotify[i];
		if ( pNotify )
		{
			pNotify->OnReqCancelStopLPResponse();
		}
	}
}

LRESULT CTradeContainerWnd::OnMsgHideTradeWnd( WPARAM w, LPARAM l )
{
	Minimize();
	return 1;
}

void CTradeContainerWnd::LoginErr()
{	
	if (ETT_TRADE_FIRM == m_stTradeLoginInfo.eTradeLoginType)
	{
		OnTradeLogIn(1);
	}
	else
	{
		OnSimulateTradeLogIn(1, m_stTradeLoginInfo.StrQuoteFlag);
	}
}

BOOL CTradeContainerWnd::PreTranslateMessage(MSG* pMsg)
{
	if (WM_SYSKEYDOWN == pMsg->message || WM_KEYDOWN == pMsg->message)
	{
		if ( VK_F12 == pMsg->wParam )
		{
			CMainFrame *pMainFrame = DYNAMIC_DOWNCAST(CMainFrame, AfxGetMainWnd());

			CIoViewBase* p = pMainFrame->FindActiveIoView();
			if (NULL != p)
			{
				pMainFrame->OnQuickTrade(2, p->GetMerchXml());
			}	
			return TRUE;
		}
	}
	
	return CRectButtonWndSimpleImpl::PreTranslateMessage(pMsg);
}

LRESULT CTradeContainerWnd::OnMsgRespTradeNotify( WPARAM w, LPARAM l )
{
	switch ((int)w)
	{
		case EMT_CONNECTED:
			{
				Login();
			}
			break;
		case EMT_DISCONNECTED: //断开
			{	
				DoOnDisConnectResponse();
			}
			break;
		case EMT_LOGINOK:
			{
				LoginOK();
			}
			break;
		case EMT_LOGOUTOK:
			{	
				//DisConnect(); 
			}
			break;
		case EMT_QUERYCODE:
			{
				DoOnQueryCodeResponse();
			}
			break;
		case EMT_USERINFO:
			{
				DoOnQueryUserInfoResponse();
			}
			break;
		case EMT_MERCHINFO:
			{
				DoOnQueryCommInfoResponse();
			}
			break;
		case EMT_QUERYTRADERID:
			{
				DoOnQueryTraderIDResponse();
			}
			break;
		case EMT_QUERYENTRUST:
			{
				DoOnQueryEntrustResponse();
			}
			break;
		case EMT_QUERYDEAL:
			{
				DoOnQueryDealResponse();
			}
			break;
		case EMT_QUERYHOLD:
			{
				DoOnQueryHoldDetailResponse();
			}
			break;
		case EMT_QUERYHOLDSUMMARY:
			{
				DoOnQueryHoldSummaryResponse();
			}
			break;
		case EMT_QUOTE:
			{
				DoOnQueryQuotationResponse();
			}
			break;
		case EMT_ENTRUST:
			{
				DoOnReqEntrustResponse();
			}
			break;
		case EMT_CANCELENTRUST:
			{
				DoOnReqCancelEntrustResponse();
			}
			break;
		case EMT_SETSTOPLP:
			{
				DoOnReqSetStopLPResponse();
			}
			break;
		case EMT_CANCELSTOPLP:
			{
				DoOnReqCancelStopLPResponse();
			}
			break;
		case EMT_MODIFY_PWD:
			{
				DoOnReqModifyPwdResponse();
			}
			break;
		case EMT_ERROR:
			{
				if (!m_bRecvMsg)
				{
					return true;
				}
				DoOnErrorResponse();
			}
			break;
	}
	return true;
}

void CTradeContainerWnd::OnMsgSimulateTradeQuotePush(IN CMerch *pMerch)
{
	if (NULL == pMerch)
	{
		return;
	}

	QueryCommInfoResultVector aCommInfo = GetCacheCommInfo();
	int32 iCommCnt = aCommInfo.size();
	QueryQuotationResultVector &aQuote = GetCacheQuotation();
	int32 iQuoteCnt = aQuote.size();
	bool32 bExist = FALSE;
	for (int32 i=0; i<iCommCnt; i++)
	{
		T_TradeMerchInfo stMerch = aCommInfo[i];
		if (pMerch->m_MerchInfo.m_StrMerchCodeInBourse == stMerch.stock_code)
		{
			bool32 bFind = false;
			for (int32 j=0; j<iQuoteCnt; j++)
			{
				T_RespQuote &stQuote = aQuote[j];
				if (stQuote.stock_code == stMerch.stock_code)
				{
					stQuote.last_price = pMerch->m_pRealtimePrice->m_fPriceNew;
					stQuote.up_price = pMerch->m_pRealtimePrice->m_fPriceHigh;
					stQuote.down_price = pMerch->m_pRealtimePrice->m_fPriceLow;
					stQuote.buy_price = pMerch->m_pRealtimePrice->m_astBuyPrices[0].m_fPrice;
					stQuote.sell_price = pMerch->m_pRealtimePrice->m_astSellPrices[0].m_fPrice;
					bFind = true;
					break;
				}
			}

			if (!bFind)
			{
				T_RespQuote stQuote;
				stQuote.stock_code = stMerch.stock_code;
				stQuote.last_price = pMerch->m_pRealtimePrice->m_fPriceNew;
				stQuote.up_price = pMerch->m_pRealtimePrice->m_fPriceHigh;
				stQuote.down_price = pMerch->m_pRealtimePrice->m_fPriceLow;
				stQuote.buy_price = pMerch->m_pRealtimePrice->m_astBuyPrices[0].m_fPrice;
				stQuote.sell_price = pMerch->m_pRealtimePrice->m_astSellPrices[0].m_fPrice;
				aQuote.push_back(stQuote);
			}
			bExist = TRUE;
			break;
		}
	}

	if (bExist)
	{
		SetQuoteValue();
	}
}

void CTradeContainerWnd::OnSimulateTradeLogIn(int32 iFlag, CString strQuoteFlag)
{
	if (1 == iFlag)
	{	
		// 1如果是从行情取价格，在登录成功后要注册推送所有交易商品的实时价格
		if (_T("1") == strQuoteFlag) // 注册推送
		{
			QueryCommInfoResultVector aCommInfo = GetCacheCommInfo();
			int32 iMerchCnt=0;
			iMerchCnt = aCommInfo.size();
			CMmiReqRealtimePrice Req;
			CMmiRegisterPushPrice ReqPush;
			CSmartAttendMerch SmartAttendMerch;

			CGGTongApp *pApp = (CGGTongApp *)AfxGetApp();
			CGGTongDoc *pDocument = pApp->m_pDocument;							
			CAbsCenterManager *pAbsCenterManager = pDocument->m_pAbsCenterManager;
			if (!pAbsCenterManager)
			{
				return;
			}

			if (0 < iMerchCnt)
			{
				CMmiReqRealtimePrice ReqRealTimePrice;
				CMmiRegisterPushPrice ReqPushPrice;

				T_TradeMerchInfo stMerch = aCommInfo[0];
				CMerch* pMerch = TradeCode2ClientMerch(stMerch.stock_code);
				if (NULL != pMerch)
				{
					ReqRealTimePrice.m_iMarketId			= pMerch->m_MerchInfo.m_iMarketId;
					ReqRealTimePrice.m_StrMerchCode		= pMerch->m_MerchInfo.m_StrMerchCode;	

					ReqPushPrice.m_iMarketId		= ReqRealTimePrice.m_iMarketId;
					ReqPushPrice.m_StrMerchCode	= ReqRealTimePrice.m_StrMerchCode;

					// 添加到关注商品列表中
					SmartAttendMerch.m_pMerch = pMerch;
					SmartAttendMerch.m_iDataServiceTypes = EDSTPrice;
					pAbsCenterManager->AddAttendMerch(SmartAttendMerch, EA_SimulateTrade);
				}

				ReqRealTimePrice.m_aMerchMore.RemoveAll();
				ReqPushPrice.m_aMerchMore.RemoveAll();

				for (int32 i=1; i<iMerchCnt; i++)
				{
					T_TradeMerchInfo stMerchInfo = aCommInfo[i];

					CMerch* pTmpMerch = TradeCode2ClientMerch(stMerchInfo.stock_code);
					if (NULL != pTmpMerch)
					{
						CMerchKey MerchKey;
						MerchKey.m_iMarketId	= pTmpMerch->m_MerchInfo.m_iMarketId;
						MerchKey.m_StrMerchCode = pTmpMerch->m_MerchInfo.m_StrMerchCode;

						ReqRealTimePrice.m_aMerchMore.Add(MerchKey);					
						ReqPushPrice.m_aMerchMore.Add(MerchKey);

						// 添加到关注商品列表中
						SmartAttendMerch.m_pMerch = pTmpMerch;
						SmartAttendMerch.m_iDataServiceTypes = EDSTPrice;
						pAbsCenterManager->AddAttendMerch(SmartAttendMerch, EA_SimulateTrade);
					}
				}

				pAbsCenterManager->RequestViewData((CMmiCommBase *)&ReqRealTimePrice);
				pAbsCenterManager->RequestViewData((CMmiCommBase *)&ReqPushPrice);
			}
		}
	}
	else
	{
		// 退出交易时是否要取消推送...

		// 取消关注商品
		CGGTongApp *pApp = (CGGTongApp *)AfxGetApp();
		CGGTongDoc *pDocument = pApp->m_pDocument;							
		CAbsCenterManager *pAbsCenterManager = pDocument->m_pAbsCenterManager;
		if (pAbsCenterManager)
		{
			pAbsCenterManager->RemoveAttendMerch(EA_SimulateTrade);
		}		
	}
}

void CTradeContainerWnd::OnTradeLogIn(int32 iFlag)
{
	// ... 其他模块相应处理
	if (1 == iFlag)
	{
		const QueryQuotationResultVector& aResult = GetCacheQuotation();
		
		for (QueryQuotationResultVector::const_iterator it = aResult.begin(); it != aResult.end(); ++it)
		{
			T_RespQuote stQuote = *it; 
			USES_CONVERSION;
			CString StrTradeCode = stQuote.stock_code;//A2W(stQuote.commodityID);
			CMerch* pMerch = TradeCode2ClientMerch(StrTradeCode);
			
			if (NULL != pMerch)
			{
				CGGTongApp *pApp = (CGGTongApp *)AfxGetApp();
				CGGTongDoc *pDocument = pApp->m_pDocument;			
				ASSERT(NULL!= pDocument);										
				CAbsCenterManager *pAbsCenterManager = pDocument->m_pAbsCenterManager;
				ASSERT(NULL!= pAbsCenterManager);
				if (!pAbsCenterManager)
				{
					return;
				}

				if (NULL != pMerch->m_pRealtimePrice)
				{
					pMerch->m_pRealtimePrice->m_fPriceNew	= stQuote.last_price;
				//	pMerch->m_pRealtimePrice->m_fPriceHigh	= stQuote.up_price;
				//	pMerch->m_pRealtimePrice->m_fPriceLow	= stQuote.down_price;
					pMerch->m_pRealtimePrice->m_astBuyPrices[0].m_fPrice = stQuote.sell_price;
					pMerch->m_pRealtimePrice->m_astSellPrices[0].m_fPrice = stQuote.buy_price;
					
					//
					pAbsCenterManager->OnRealtimePrice(*pMerch->m_pRealtimePrice, -1, -1);
					TRACE(L"请求回 交易商品[%s - %s] 的行情数据: new: %.2f high: %.2f low: %.2f buy: %.2f sell: %.2f \n", pMerch->m_MerchInfo.m_StrMerchCode.GetBuffer(), pMerch->m_MerchInfo.m_StrMerchCodeInBourse.GetBuffer(), stQuote.last_price, stQuote.up_price, stQuote.down_price, stQuote.buy_price, stQuote.sell_price);
					pMerch->m_MerchInfo.m_StrMerchCode.ReleaseBuffer();
					pMerch->m_MerchInfo.m_StrMerchCodeInBourse.ReleaseBuffer();
				}
				else
				{
					// 还没请求过这个行情数据, 请求一下
					CMmiReqRealtimePrice Req;
					Req.m_iMarketId		= pMerch->m_MerchInfo.m_iMarketId;
					Req.m_StrMerchCode	= pMerch->m_MerchInfo.m_StrMerchCode;

					pAbsCenterManager->RequestViewData(&Req);
				}					
			}
		}
	}
		
}

void CTradeContainerWnd::OnTradeQuotationPush(const CClientRespQuote *pAns, const int iCount)
{
	if (NULL == pAns || iCount <= 0)
	{
		//ASSERT(0);
		return;
	}

	//
	for (int32 i = 0; i < iCount; i++)
	{
		CMerch* pMerch = TradeCode2ClientMerch(pAns->m_aQuote[i].stock_code);
		
		if (NULL != pMerch)
		{
			CGGTongApp *pApp = (CGGTongApp *)AfxGetApp();
			CGGTongDoc *pDocument = pApp->m_pDocument;			
			ASSERT(NULL!= pDocument);										
			CAbsCenterManager *pAbsCenterManager = pDocument->m_pAbsCenterManager;
			if (!pAbsCenterManager)
			{
				return;
			}

			ASSERT(NULL!= pAbsCenterManager);

			if (NULL != pMerch->m_pRealtimePrice)
			{
				pMerch->m_pRealtimePrice->m_fPriceNew	= pAns->m_aQuote[i].last_price;
			//	pMerch->m_pRealtimePrice->m_fPriceHigh	= pAns->m_aQuote[i].up_price;
			//	pMerch->m_pRealtimePrice->m_fPriceLow	= pAns->m_aQuote[i].down_price;
				pMerch->m_pRealtimePrice->m_astBuyPrices[0].m_fPrice = pAns->m_aQuote[i].sell_price;
				pMerch->m_pRealtimePrice->m_astSellPrices[0].m_fPrice = pAns->m_aQuote[i].buy_price;
				
				//
				TRACE(L"推送 交易商品[%s - %s] 的行情数据: new: %.2f high: %.2f low: %.2f buy: %.2f sell: %.2f \n", pMerch->m_MerchInfo.m_StrMerchCode.GetBuffer(), pMerch->m_MerchInfo.m_StrMerchCodeInBourse.GetBuffer(), pAns->m_aQuote[i].last_price, pAns->m_aQuote[i].up_price, pAns->m_aQuote[i].down_price, pAns->m_aQuote[i].buy_price, pAns->m_aQuote[i].sell_price);
				pMerch->m_MerchInfo.m_StrMerchCode.ReleaseBuffer();
				pMerch->m_MerchInfo.m_StrMerchCodeInBourse.ReleaseBuffer();
				pAbsCenterManager->OnRealtimePrice(*pMerch->m_pRealtimePrice, -1, -1);
			}
			else
			{
				// 还没请求过这个行情数据, 请求一下
				CMmiReqRealtimePrice Req;
				Req.m_iMarketId		= pMerch->m_MerchInfo.m_iMarketId;
				Req.m_StrMerchCode	= pMerch->m_MerchInfo.m_StrMerchCode;
				

				pAbsCenterManager->RequestViewData(&Req);
			}			
		}
	}
}

CMerch* CTradeContainerWnd::TradeCode2ClientMerch(const CString& StrTradeCode)
{
	CGGTongApp *pApp = (CGGTongApp *)AfxGetApp();
	CGGTongDoc *pDocument = pApp->m_pDocument;			
	ASSERT(NULL!= pDocument);				

	CAbsCenterManager *pAbsCenterManager = pDocument->m_pAbsCenterManager;
	if (!pAbsCenterManager)
	{
		return NULL;
	}
	ASSERT(NULL!= pAbsCenterManager);


	CMerch* pMerch = NULL;
	pAbsCenterManager->GetMerchManager().FindTradeMerch(StrTradeCode, pMerch);
	return pMerch;
}
//
CString	CTradeContainerWnd::ClientMerch2TradeCode(const CMerch* pMerch)
{
	if (NULL == pMerch)
	{		
		const QueryCommInfoResultVector& aResult = GetCacheCommInfo();
		if (0 < aResult.size())
		{
			return aResult[0].stock_code;
		}
		return L"";
	}
	//
	return pMerch->m_MerchInfo.m_StrMerchCodeInBourse;	
}