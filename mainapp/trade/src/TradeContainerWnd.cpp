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
	static const int		kiTimerIdDebugLoginTestCount = 5;		// n ��timer������

	static const int		kiTimerIdHeartBeat = 110;			// ������
	static const int		kiTimerPeriodHeartBeat = 10*1000;	//

	static const int		kiTimerIdTradeLoginTimeout = 111;			// ��¼��ʱ
	static const int		kiTimerPeriodTradeLoginTimeout = 45*1000;	//

	static const int		kiTimerPeriodDebugLoginTest = 1000;

	static const int		kiBtnWidth = 24;
	static const int		kiBtnSeparatorWidth = 2;

	static const char		k_szCelueName[] = "TradePoly";
	static const char		k_szMyName[] = "TradeWindow";

	void	DrawVerticalText(CDC &dc, const CString &Str, CRect rcDraw)
	{
		// ������д����
		CString StrTest(_T("��׼���"));
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
	m_pBtnClose = new CWndRectButton(*this, this, KiBtnCloseID, CWndRectButton::Marlett_Close, _T("�˳�����"));
	m_pBtnMax = new CWndRectButton(*this, this, KiBtnMaxID, CWndRectButton::Marlett_MAX, _T("���"));
	m_pBtnMin = new CWndRectButton(*this, this, KiBtnMinID, CWndRectButton::Marlett_Min, _T("��С��"));

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

	// ���ɵ�¼��ť
	m_BtnLogin.Create(L"���׵�¼", WS_CHILD | BS_DEFPUSHBUTTON, CRect(0, 0, 100, 30), this, IDC_BUTTON_TRADELOGIN_SELF);
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
	// ���Ʊ���
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
// 	// ��ť����
// 	CRectButtonWndSimpleImpl::OnDraw(dc);
// 
// 	// ��ť�����������
// 	COLORREF clrText = GetSysColor(COLOR_WINDOWTEXT);
// 	CFont fontTitle;
// 	LOGFONT lg = {0};
// 	lg.lfHeight = -14;
// 	lg.lfCharSet = GB2312_CHARSET;
// 	lg.lfWeight = 700;
// 	_tcscpy(lg.lfFaceName, _T("����"));
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
// 	CString StrTitle(_T("�ڣģ�ɵ�Ͻ���ϵͳ"));
// 	DrawVerticalText(dc, StrTitle, rcTitle);
}

void CTradeContainerWnd::RecalcLayout( bool32 bNeedPaint /*= true*/ )
{
	const int iBtnWidth = kiBtnWidth;

	CRect rc(0,0,0,0);
	GetClientRect(rc);

	if ( rc.Width()>iBtnWidth )
	{
		// �����ӿؼ�����~~
		CRect rcOther(rc);

 		m_pDlgTradeBidParent->MoveWindow(rcOther);

// 		// ��¼������ʾλ��
// 		CRect rcDlgLogin(rc);
// 		rcDlgLogin.left = (rc.right-rc.left)/2;
// 		rcDlgLogin.top = rc.top+50;
// // 		rcDlgLogin.right -= iBtnWidth;
// // 		rcDlgLogin.right -= kiBtnSeparatorWidth;
// 		m_pDlgLogin->MoveWindow(rcDlgLogin);

		// ����¼��ť
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
		CString StrQues = _T("�Ƿ��˳����ף�");
		if ( IDYES!=MessageBox(StrQues, _T("��ʾ"), MB_ICONQUESTION|MB_YESNO) )
		{
			return;
		}

		if ( NULL!=m_pLoginNotify )
		{
			m_pLoginNotify->OnLoginStep(_T("���׵�¼."));
		}
	}
	// ���׹ر�
	DoStopTrade();

	// �رպ���С��
	Minimize();
}

void CTradeContainerWnd::OnTradeMax()
{
	if ( IsMaximized() )
	{
		// ��ť�滻�����
		CString Str;
		Str += (TCHAR)(CWndRectButton::Marlett_MAX);
		m_pBtnMax->SetBtnText(Str);
		RestoreSize();
		// ��ʾ���
	}
	else
	{
		// ��ť�滻�ɻ�ԭ
		CString Str;
		Str += (TCHAR)(CWndRectButton::Marlett_RESTORE);
		m_pBtnMax->SetBtnText(Str);
		Maximize();
		// ��ʾ���
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
		// ��ʾ��С��
		Minimize();
	}	
}

void CTradeContainerWnd::OnBtnClick( NMHDR *pHdr, LRESULT *pResult )
{
	// ������ǰ�ĸ�ʽת����wndParent
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
	if (0 != m_stTradeLoginInfo.m_proxyInfol.uiProxyType) // �д���
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
		CString StrErrMsg = _T("���ӷ�����ʧ��");
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
	m_eLoginStatus = ETLS_Logining;	// ���ڵ�½
	m_pLoginNotify->OnLoginStep(_T("��ʼ��¼..."));
	bool32 bRet;
	CClientReqLogIn reqLogIn;
	reqLogIn.account = m_stTradeLoginInfo.StrUser;
	reqLogIn.password = m_stTradeLoginInfo.StrPwd;
	bRet = m_tradeClient->ReqLogIn(reqLogIn);

	if (!bRet)
	{
		CString StrErrMsg = _T("�û���¼ʧ��");
		m_pLoginNotify->OnLoginError(StrErrMsg);
	 	m_pLoginNotify->OnUpdateValidCode();
		DisConnect(); // �Ͽ�����
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
		CString StrErrMsg = _T("������δ����sessionID");
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
		//	if (!(pNotify->OnInitQuery(StrErrMsg)))// ��ʼ����ѯ
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

	// Ҫ��ֹͣ��¼
	m_pLoginNotify = NULL;
}

void CTradeContainerWnd::OnLoginDlgCancel()
{
	DoStopTrade();

	// Ҫ��ֹͣ��¼
	m_pLoginNotify = NULL;

	if (!IsLogin())	//�϶���û�е�¼��״̬
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
		// ��ʾ���׽���
	}
	else
	{
		// ��ʾ��¼
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
// 		// ��¼��ʱ
// 		_AnsHead stHead;
// 		stHead.uStatus = -1;
// 		CString StrErr = _T("��¼��ʱ�������³���.");
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
// 	// ������־
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
// 	// ˢ��Ʒ����Ϣ
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
// 	DoOnLog(k_szMyName, celueCore::ELTNormal, "���뽻��ϵͳ.");
// 
// 	DWORD dwTime2 = timeGetTime();
// 	TRACE(_T("��¼ʱ��: %d\r\n"), dwTime2-dwTime);
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
 	// ������Ϣ���
	m_aCacheHoldDetail.clear();
	m_aCacheEntrust.clear();
	m_aCacheDeal.clear();
	m_aCacheCommInfo.clear();
	m_aCacheQuotation.clear();
	m_aCacheTraderID.clear();

	// 	// ������־
	// 	DoOnLog(k_szMyName, celueCore::ELTNormal, "�˳�����ϵͳ.\r\n");
// 	CTradeLogSave::Instance().Save();

 	SetLoginStatus(ETLS_NotLogin);

	//�ǳ�֪ͨ
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
		// ��ʾ���׽���
		m_pDlgLogin->ShowWindow(SW_HIDE); 
		
		m_pDlgTradeBidParent->ShowWindow(SW_SHOW);
		m_pDlgTradeBidParent->SetFocus();
	//	m_BtnLogin.ShowWindow(SW_HIDE);
	}
	else
	{
		// ��ʾ��¼
		m_pDlgLogin->ShowWindow(SW_SHOW);
		m_pDlgLogin->SetFocus();
	
		m_pDlgTradeBidParent->ShowWindow(SW_HIDE);
	//	Minimize();
	//	m_BtnLogin.ShowWindow(SW_SHOW);
	}
}

void CTradeContainerWnd::OnBtnTradeLogin()	// ��¼����
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
	// ��ȡ�ֲ���ϸ��Ϣ
	bool32 bRet;
	bRet = m_tradeClient->ReqQueryHold(stReq);

	if (!bRet)
	{
		strErrMsg.Format(_T("��ѯ�ֲ���ϸʧ��"));
		if (ETLS_Logined != m_eLoginStatus)	// ��¼��
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
		strErrMsg.Format(_T("��ѯ�ֲֻ���ʧ��"));
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
	// ��ȡί����Ϣ
	bool32 bRet;
	bRet = m_tradeClient->ReqQueryEntrust(stReq);

	if (!bRet)
	{
		strErrMsg.Format(_T("��ѯί��ʧ��"));
		if (ETLS_Logined != m_eLoginStatus)	// ��¼��
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
		strErrMsg.Format(_T("��ѯί��ʧ��"));
		if (ETLS_Logined != m_eLoginStatus)	// ��¼��
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
	// ��ȡ�ɽ���Ϣ
	bool32 bRet;
	bRet = m_tradeClient->ReqQueryDeal(stReq);

	if (!bRet)
	{
		strErrMsg.Format(_T("��ѯƽ�ֵ�ʧ��"));
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
		strErrMsg.Format(_T("��ѯ�Է�����Աʧ��"));
		if (ETLS_Logined != m_eLoginStatus)	// ��¼��
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
	// ��ȡ��Ʒ��Ϣ
	bool32 bRet;
	bRet = m_tradeClient->ReqMerchInfo(stReq);
	
	if ( !bRet)
	{
		strErrMsg.Format(_T("��ѯ��Ʒ��Ϣʧ��"));
		if (ETLS_Logined != m_eLoginStatus)	// ��¼��
		{
			if (NULL != m_pLoginNotify)
			{
				m_pLoginNotify->OnLoginError(strErrMsg);
			}
		}

		return bRet;
	}

	if (ETLS_Logined != m_eLoginStatus)	// ��¼��
	{
		if (NULL != m_pLoginNotify)
		{
			m_pLoginNotify->OnLoginStep(_T("���ڼ�����Ʒ��Ϣ......"));
		}
	}
	
	return bRet;
}

bool32 CTradeContainerWnd::ReqUserInfo(const CClientReqUserInfo& stReq, CString &strErrMsg)
{
	// ��ȡ�˻���Ϣ��Ϣ
	bool32 bRet;
	bRet = m_tradeClient->ReqUserInfo(stReq);
	
	if ( !bRet)
	{
		strErrMsg.Format(_T("��ѯ�˻���Ϣʧ��"));	
		if (ETLS_Logined != m_eLoginStatus)	// ��¼��
		{
			if (NULL != m_pLoginNotify)
			{
				m_pLoginNotify->OnLoginError(strErrMsg);
			}
		}
		return bRet;
	}

	if (ETLS_Logined != m_eLoginStatus)	// ��¼��
	{
		if (NULL != m_pLoginNotify)
		{
			m_pLoginNotify->OnLoginStep(_T("���ڼ����˻���Ϣ......"));
		}
	}
	
	return bRet;
}

// ί������
bool32 CTradeContainerWnd::ReqEntrust(const CClientReqEntrust& stReq, CString &strTipMsg)
{
	bool32 bRet;
	bRet = m_tradeClient->ReqEntrust(stReq);

	if ( !bRet)
	{
		strTipMsg.Format(_T("����ʧ��!"));
	}
	else
	{
		strTipMsg.Format(_T("�����ɹ�!"));
	}
	
	return bRet;
}

// ������ί�е�
bool32 CTradeContainerWnd::ReqCancelEntrust(const CClientReqCancelEntrust& stReq, CString &strTipMsg)
{
	bool32 bRet;
	bRet = m_tradeClient->ReqCancelEntrust(stReq);

	if ( !bRet)
	{
		strTipMsg.Format(_T("����ʧ��!"));		
	}
	else
	{
		strTipMsg.Format(_T("�����ɹ�!"));
	}
	
	return bRet;
}

// ��ֹ��ֹӯ��
bool32 CTradeContainerWnd::ReqCancelStopLP(const CClientReqCancelStopLP& stReq, CString &strTipMsg)
{
	bool32 bRet;
	bRet = m_tradeClient->ReqCancelStopLP(stReq);

	if ( !bRet)
	{
		strTipMsg.Format(_T("����ʧ��!"));		
	}
	else
	{
		strTipMsg.Format(_T("�����ɹ�!"));
	}
	
	return bRet;
}

// ����ֹӯֹ��
bool32 CTradeContainerWnd::ReqSetStopLP(const CClientReqSetStopLP& stReq, CString &strTipMsg)
{
	bool32 bRet;
	bRet = m_tradeClient->ReqSetStopLP(stReq);

	if ( !bRet)
	{
		strTipMsg.Format(_T("����ʧ��!"));		
	}
	else
	{
		strTipMsg.Format(_T("�����ɹ�!"));
	}
	
	return bRet;
}

// �����޸�����
bool32 CTradeContainerWnd::ReqModifyPwd(const CClientReqModifyPwd& stReq, CString &strTipMsg)
{
	bool32 bRet;
	bRet = m_tradeClient->ReqModifyPwd(stReq);
	
	if ( !bRet)
	{
		strTipMsg.Format(_T("����ʧ��!"));		
	}
	else
	{
		strTipMsg.Format(_T("�����ɹ�!"));
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
// 			strTipMsg.Format(_T("����ʧ��!"));		
// 		}
// 		
// 		bSuc = false;
// 	}
// 	else
// 	{
// 		strTipMsg.Format(_T("�����ɹ�!"));
// 		bSuc = true;
// 	}
// 	
// 	// �ͷ��ڴ�
// 	autho.Release(pMsg);
// 	
// 	return iRet;	
}

// �����С��
void CTradeContainerWnd::WndMaxMin()
{
	const int KLenTitleTop = 35;	// ����ȥ�ĸ߶�
	
	CMyContainerBar *pParentBar = GetParentBar();
	if (NULL==pParentBar)
	{
		return;
	}

	CSize size = pParentBar->GetSizeDefault();

	if (m_bWndMaxMinStatus)//��ǰ�����״̬(�������С��ť)
	{
		m_MaxSize = pParentBar->GetCurrentSize();
		m_bWndMaxMinStatus = FALSE;
		size.cy = KLenTitleTop;
		pParentBar->SetFixedHeight(true,size.cy);	//�趨�̶��߶�
		pParentBar->SetNewSize(size);
	}
	else
	{
		m_bWndMaxMinStatus = TRUE;
		pParentBar->SetFixedHeight(false,m_MaxSize.cy);	//���趨�̶��߶�
		pParentBar->SetNewSize(m_MaxSize);
	}
}

// �˳�����
void CTradeContainerWnd::WndClose(int iMode)
{
	if (2==iMode)//���̨����ע������
	{
		CString strName = m_stTradeLoginInfo.StrUser;
		CString strSessionID = m_stTradeLoginInfo.StrUserSessionID;
		CString strSessionInfo = m_stTradeLoginInfo.StrUserSessionInfo;

		m_tradeClient->ReqLogOut(strName, strSessionID, strSessionInfo); //����ע���ɹ���ʧ�ܣ����˳�����
	}

	if ( IsLogin()&&iMode!=3 )
	{		
		if ( NULL!=m_pLoginNotify )
		{
			m_pLoginNotify->OnLoginStep(_T("���׵�¼"));
		}
	}
	// �رս���
	StopLogin(iMode);//pTrade();
	
	// �رպ���С��
	if (0==iMode)//���µ�¼
	{
		TryLogin();
	}
// 	else//ע��
// 	{
// 		Minimize();
// 	}
	Minimize();
}

// ������
bool32 CTradeContainerWnd::ReqQute(const CClientReqQuote& stReq, CString &strErrMsg)
{
	bool32 bRet;
	bRet = m_tradeClient->ReqQute(stReq);
	
	if ( !bRet)
	{
		strErrMsg.Format(_T("��ѯ������Ϣʧ��"));
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
			pNotify->OnQueryHoldDetailResponse();// ���ֲ���ϸ���
		}
	}
	if (m_MaxRespCnt > iRespCnt)
	{
		iRespCnt++;
		if (ETLS_Logined != m_eLoginStatus)	// ��¼��
		{
			if (NULL != m_pLoginNotify)
			{
				m_pLoginNotify->OnLoginStep(_T("���ڼ��سֲֻ���......"));
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
		ShowProperChild();	// ֪ͨ��ʾ���׽��棬���ص�½����
		
		//֪ͨ���׽�����ʾ
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
			pNotify->OnQueryHoldSummaryResponse();// ���ֲֻ��ܱ��
		}
	}

	if (m_MaxRespCnt > iRespCnt)
	{
		iRespCnt++;
		if (ETLS_Logined != m_eLoginStatus)	// ��¼��
		{
			if (NULL != m_pLoginNotify)
			{
				m_pLoginNotify->OnLoginStep(_T("���ڼ���ָ��ί�е�......"));
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
		ShowProperChild();	// ֪ͨ��ʾ���׽��棬���ص�½����

		//֪ͨ���׽�����ʾ
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
		if (ETLS_Logined != m_eLoginStatus)	// ��¼��
		{
			if (NULL != m_pLoginNotify)
			{
				m_pLoginNotify->OnLoginStep(_T("���ڼ���ƽ�ֵ�......"));
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
		ShowProperChild();	// ֪ͨ��ʾ���׽��棬���ص�½����
		
		//֪ͨ���׽�����ʾ
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
		ShowProperChild();	// ֪ͨ��ʾ���׽��棬���ص�½����
		
		//֪ͨ���׽�����ʾ
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
		if (ETLS_Logined != m_eLoginStatus)	// ��¼��
		{
			if (NULL != m_pLoginNotify)
			{
				m_pLoginNotify->OnLoginStep(_T("���ڼ��سֲ���ϸ......"));
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
		ShowProperChild();	// ֪ͨ��ʾ���׽��棬���ص�½����
		
		//֪ͨ���׽�����ʾ
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
		if (ETLS_Logined != m_eLoginStatus)	// ��¼��
		{
			if (NULL != m_pLoginNotify)
			{
				m_pLoginNotify->OnLoginStep(_T("���ڼ��ضԷ�����ԱID......"));
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
		ShowProperChild();	// ֪ͨ��ʾ���׽��棬���ص�½����

		//֪ͨ���׽�����ʾ
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
				if (!(pNotify->OnInitQuery(StrErrMsg)))// ��ʼ����ѯ
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
		//���������ͽ�����������
		OnTradeQuotationPush(&pAns,m_aCacheQuotation.size());
	}
}

void CTradeContainerWnd::DoOnErrorResponse()
{
	m_stCacheError = m_respTradeNotify->GetCacheError();
	if (ETLS_Logining == m_eLoginStatus)  // ���ڵ�¼
	{
		m_eLoginStatus = ETLS_NotLogin;
		m_pLoginNotify->OnLoginError(m_stCacheError.error_info);
		m_pLoginNotify->OnUpdateValidCode();
		m_stCacheError.error_no = 0;
		m_stCacheError.error_info = L"";
		return;
	}

	if (-1 == m_stCacheError.error_no) //�ʺ��������ط���¼
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
		dlg.m_pCenterWnd = this;	// ����Ҫ���е��Ĵ���ָ��		
		dlg.DoModal();

		m_pDlgTradeBidParent->PostMessage(WM_CLOSETRADE,0,0);	//֪ͨ������������

		WndClose(2);  // �������µ�¼
		TryLogin();
	}
}

void CTradeContainerWnd::DoOnDisConnectResponse()
{
	if (ETLS_Logining == m_eLoginStatus)// ���ڵ�¼
	{
		m_eLoginStatus = ETLS_NotLogin;
		m_pLoginNotify->OnLoginError(_T("����������ӶϿ�"));
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
		case EMT_DISCONNECTED: //�Ͽ�
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
		// 1����Ǵ�����ȡ�۸��ڵ�¼�ɹ���Ҫע���������н�����Ʒ��ʵʱ�۸�
		if (_T("1") == strQuoteFlag) // ע������
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

					// ��ӵ���ע��Ʒ�б���
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

						// ��ӵ���ע��Ʒ�б���
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
		// �˳�����ʱ�Ƿ�Ҫȡ������...

		// ȡ����ע��Ʒ
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
	// ... ����ģ����Ӧ����
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
					TRACE(L"����� ������Ʒ[%s - %s] ����������: new: %.2f high: %.2f low: %.2f buy: %.2f sell: %.2f \n", pMerch->m_MerchInfo.m_StrMerchCode.GetBuffer(), pMerch->m_MerchInfo.m_StrMerchCodeInBourse.GetBuffer(), stQuote.last_price, stQuote.up_price, stQuote.down_price, stQuote.buy_price, stQuote.sell_price);
					pMerch->m_MerchInfo.m_StrMerchCode.ReleaseBuffer();
					pMerch->m_MerchInfo.m_StrMerchCodeInBourse.ReleaseBuffer();
				}
				else
				{
					// ��û����������������, ����һ��
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
				TRACE(L"���� ������Ʒ[%s - %s] ����������: new: %.2f high: %.2f low: %.2f buy: %.2f sell: %.2f \n", pMerch->m_MerchInfo.m_StrMerchCode.GetBuffer(), pMerch->m_MerchInfo.m_StrMerchCodeInBourse.GetBuffer(), pAns->m_aQuote[i].last_price, pAns->m_aQuote[i].up_price, pAns->m_aQuote[i].down_price, pAns->m_aQuote[i].buy_price, pAns->m_aQuote[i].sell_price);
				pMerch->m_MerchInfo.m_StrMerchCode.ReleaseBuffer();
				pMerch->m_MerchInfo.m_StrMerchCodeInBourse.ReleaseBuffer();
				pAbsCenterManager->OnRealtimePrice(*pMerch->m_pRealtimePrice, -1, -1);
			}
			else
			{
				// ��û����������������, ����һ��
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