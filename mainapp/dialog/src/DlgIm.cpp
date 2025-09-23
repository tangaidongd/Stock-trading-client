// CDlgIm.cpp : implementation file
//

#include "stdafx.h"
#include <Tlhelp32.h>
#include "DlgIm.h"
#include "StatusBarEx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgIm dialog

#define WM_TASKTRAY_MESSAGE	WM_USER + 125

const unsigned int RET_AUTH_OK = 0;						//返回成功
const unsigned int RET_AUTH_ACCT_NOTEXIST = 0x00080001;	//帐号不存在
const unsigned int RET_AUTH_ACCT_DISABLE = 0x00080002;	//帐号被禁用
const unsigned int RET_AUTH_ACCT_NOTACT = 0x00080003;	//帐号未激活
const unsigned int RET_AUTH_ACCT_FREEZ = 0x00080004;	//帐号被冻结
const unsigned int RET_AUTH_PWD_ERROR = 0x00080005;		//密码错误
const unsigned int RET_AUTH_TICKET_INVALID = 0x00080006;//票据不正确
const unsigned int RET_AUTH_TICKET_EXPIRE = 0x00080007;	//票据正确但已过期
const unsigned int RET_AUTH_SYSTEM_ERROR = 0x00080008;	//认证服务器错误
const unsigned int RET_AUTH_PROTOCOL_ERROR = 0x00080009;//认证协议不正确
const unsigned int RET_AUTH_TIMEOUT = 0x0008000a;		//认证处理超时
const unsigned int RET_AUTH_DP_ERROR = 0x0008000b;		//数据平台错误
const unsigned int RET_AUTH_PWD_PROTECT_NOTEXIST = 0x0008000c;//密码保护信息不存在
const unsigned int RET_AUTH_PWD_EXPIRE = 0x0008000d;	//密码已过期(华股街导入的帐号，必须修改密码)
const unsigned int RET_AUTH_UNKNOWN = 0xffffff;	


CDlgIm::CDlgIm(CWnd* pParent)
	: CDialog(CDlgIm::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgIm)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_iWnd = 0;
	m_blShow     = false;
	m_blExeExist = false;
	m_RectWindow = CRect(0,0,0,0);
	m_ImTrayWnd = NULL;
}

void CDlgIm::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgIm)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDlgIm, CDialog)
	//{{AFX_MSG_MAP(CDlgIm)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_CLOSE()
	ON_WM_TIMER()
	//IM通知
	ON_MESSAGE(WM_IM_NOTIFY, OnImNotify)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//#define IM_PATH  _T("IM\\ImBinDebug\\")
#define IM_PATH  _T("IM\\ImBin\\")

const int KTimerIdCheckLogin  = 101010; //timerid
const int KTimerPeriodCheckLogin = 30000; //15秒

/////////////////////////////////////////////////////////////////////////////
// CDlgIm message handlers
void CDlgIm::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// Do not call CDialog::OnPaint() for painting messages
	if ( m_blShow )
	{
		ShowWindow(SW_SHOW);
	}
	else
	{
		ShowWindow(SW_HIDE);
	}
}

void CDlgIm::OnClose() 
{
	CMainFrame* pWnd    = (CMainFrame*)AfxGetMainWnd();
	pWnd->m_pDlgIm = NULL;

	KillProcessFromName("ATFIM.exe");
	DestroyWindow();
}

void CDlgIm::PostNcDestroy() 
{	
	CDialog::PostNcDestroy();
	delete this;
}

BOOL CDlgIm::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// SetWindowPos(&wndTopMost,0,0,0,0,SWP_NOMOVE);	
	// ShowCursor(FALSE);
	ShowWindow(SW_HIDE);
	
	//CMainFrame* pWnd  = (CMainFrame*)AfxGetMainWnd();
	//HWND hPar = pWnd->GetSafeHwnd();
	
	//
	m_mapAccount.insert(pair<CString, CString>(L"10755924", L"qsgbs_f@jwxt.com"));
	m_mapAccount.insert(pair<CString, CString>(L"10110784", L"qsgbs_g@jwxt.com"));

	m_mapAccount.insert(pair<CString, CString>(L"10696953", L"qsgbs_client1@jwxt.com"));
	m_mapAccount.insert(pair<CString, CString>(L"10416663", L"qsgbs_client2@jwxt.com"));
	m_mapAccount.insert(pair<CString, CString>(L"10126003", L"qsgbs_client3@jwxt.com"));
	m_mapAccount.insert(pair<CString, CString>(L"10556663", L"qsgbs_client4@jwxt.com"));
	m_mapAccount.insert(pair<CString, CString>(L"10085563", L"qsgbs_client5@jwxt.com"));
	m_mapAccount.insert(pair<CString, CString>(L"10612273", L"qsgbs_client6@jwxt.com"));
	m_mapAccount.insert(pair<CString, CString>(L"10777233", L"qsgbs_client7@jwxt.com"));
	m_mapAccount.insert(pair<CString, CString>(L"10162603", L"qsgbs_client8@jwxt.com"));
	//
	CString strCurPath;
	GetModuleFileName(NULL,strCurPath.GetBuffer(MAX_PATH),MAX_PATH);
	strCurPath.ReleaseBuffer();
	int pos= strCurPath.ReverseFind(_T('\\'));
	m_strFullPath =  strCurPath.Mid(0, pos + 1);

	if ( m_strFullPath.IsEmpty() )
		return TRUE;

	m_strFullPath += IM_PATH;

	CString strFileName = m_strFullPath + L"SafeHwnd.txt";
	CString strContent = L" ";

	CString strParam;
	HWND hPar  = GetSafeHwnd();

	strContent.Format(L"%u", hPar);

	CStdioFile file; 
	if ( !file.Open(strFileName, CFile::modeCreate|CFile::modeWrite | CFile::typeText) )
		return TRUE;

	file.WriteString(strContent);
	file.WriteString(_T("\r\n"));
	file.Close();
	
	CString strExe = m_strFullPath + _T("ATFIM.exe");

	if( INVALID_FILE_ATTRIBUTES != ::GetFileAttributes(strExe) )
	{
		m_blExeExist = true;
	}
	
	m_ImTrayWnd = NULL;
	
	m_strPwd = L"";
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CDlgIm::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;
}

BOOL CDlgIm::PreTranslateMessage(MSG* pMsg) 
{
	if ( WM_KEYDOWN == pMsg->message)
	{
		if (VK_ESCAPE == pMsg->wParam || VK_RETURN == pMsg->wParam)
		{
			PostMessage(WM_CLOSE,0,0);
		}
	}	
	return CDialog::PreTranslateMessage(pMsg);
}

//显示或隐藏IM
void CDlgIm::ShowIm(bool blShow)
{
	m_blShow = blShow;

	if ( m_blShow )
	{
		//MoveWindow(m_RectWindow, FALSE);
		ShowWindow(SW_SHOW);
	}
	else
		ShowWindow(SW_HIDE);
}

bool CDlgIm::IsExist()
{
	return m_blExeExist;
}

void CDlgIm::MoveIm(CRect rect)
{
	MoveWindow(&rect, TRUE);
}

/*
//IM暴露时，改变窗体大小
void CDlgIm::ChangeSize()
{
	if ( m_blShow )
	{
		MoveWindow(m_RectWindow, FALSE);
		ShowWindow(SW_SHOW);
	}
}

void CDlgIm::SetWinRect(const CRect *pRect)
{
	m_RectWindow = *pRect;
}

void CDlgIm::GetWinRect(CRect &rect)
{
	rect = m_RectWindow;
}
*/

//开始登录
void CDlgIm::OnLogin(CString strName)
{
	if ( strName.IsEmpty() )
		return;

	//m_strName = strName; 
	
	if ( !m_blExeExist )
		return;

	m_strName  = strName;
	m_strName.TrimLeft();
	m_strName.TrimRight();
	m_strPwd   = L"IM123456";

	//
	map<CString, CString>::iterator  iterAccount, iterPwd;

	if ( m_mapAccount.end() != (iterAccount = m_mapAccount.find(m_strName)) )
	{
		m_strName = iterAccount->second; //获取帐号
	}

	CString strParam;
	HWND hPar  = GetSafeHwnd();

	KillProcessFromName("ATFIM.exe");
	Sleep(1000);
	strParam = L" ";
	strParam.Format(L"/handle %lu /account %s /password %s",hPar, m_strName.GetBuffer(), m_strPwd.GetBuffer());
//	::ShellExecute(NULL,L"open",L"D:\\工作目录\\IM项目\\original\\src\\IM1.0\\Output\\BinDebug\\DyjIM.exe",strParam,NULL,SW_SHOW);
//	m_strFullPath = L"D:\\工作目录\\IM项目\\original\\src\\IM1.0\\Output\\BinDebug\\";
	CString strExe = m_strFullPath + _T("ATFIM.exe");
	::ShellExecute(NULL,L"open",strExe,strParam,NULL,SW_SHOW);
	
	SetTimer(KTimerIdCheckLogin, KTimerPeriodCheckLogin, NULL);

#if 0 //Test 2013-10-31 by cym
	OnImNotify(NULL, NULL);
#endif
}

BOOL CDlgIm::KillProcessFromName(LPCSTR lpProcessName)
{
	//创建进程快照(TH32CS_SNAPPROCESS表示创建所有进程的快照)
	HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
	
	//PROCESSENTRY32进程快照的结构体
	PROCESSENTRY32 pe;
	
	//实例化后使用Process32First获取第一个快照的进程前必做的初始化操作
	pe.dwSize = sizeof(PROCESSENTRY32);
	
	
	//下面的IF效果同:
	//if(hProcessSnap == INVALID_HANDLE_VALUE)   无效的句柄
	if(!Process32First(hSnapShot,&pe))
	{
		return FALSE;
	}
	CString strProcessName = lpProcessName;
	
	//将字符串转换为小写
	strProcessName.MakeLower();
	
	
	//如果句柄有效  则一直获取下一个句柄循环下去
	while (Process32Next(hSnapShot,&pe))
	{
		
		//pe.szExeFile获取当前进程的可执行文件名称
		CString scTmp = pe.szExeFile;
		
		
		//将可执行文件名称所有英文字母修改为小写
		scTmp.MakeLower();
		
		//比较当前进程的可执行文件名称和传递进来的文件名称是否相同
		//相同的话Compare返回0
		if(!scTmp.Compare(strProcessName))
		{
			
			//从快照进程中获取该进程的PID(即任务管理器中的PID)
			DWORD dwProcessID = pe.th32ProcessID;
			HANDLE hProcess = ::OpenProcess(PROCESS_TERMINATE,FALSE,dwProcessID);
			::TerminateProcess(hProcess,0);
			DEL_HANDLE(hProcess);
			return TRUE;
		}
		scTmp.ReleaseBuffer();
	}

	strProcessName.ReleaseBuffer();

	return FALSE;
}

//获取登录错误代码
unsigned int CDlgIm::LastLoginCode()
{
	
	if ( m_strFullPath.IsEmpty() )
		return RET_AUTH_OK;

	CStdioFile file; 
	CString strFileName = m_strFullPath + L"Login_Log\\LoginError.txt";

	if ( !file.Open(strFileName, CFile::modeRead | CFile::typeText) )
		return RET_AUTH_OK;
	
	CString strConent;

	file.ReadString(strConent);

	int iBegin = strConent.Find(_T("ErrCode:"), 0);
	int iEnd  = strConent.Find(_T(" "), iBegin);
	
	if ( -1 == iBegin || -1 == iEnd)
		return RET_AUTH_UNKNOWN;

	strConent = strConent.Mid(iBegin + 8 , iEnd - iBegin - 8);

	file.Close();

	return _wtoi(strConent);
}

void CDlgIm::OnTimer(UINT nIDEvent)
{
	if ( KTimerIdCheckLogin == nIDEvent )
	{
		static int gIcheckNume = 1; //5次登录检查

		int errCode;
		errCode = LastLoginCode();
		
		switch (errCode)
		{
			case RET_AUTH_ACCT_NOTEXIST:
			case RET_AUTH_PWD_ERROR:
			case RET_AUTH_ACCT_DISABLE:
			case RET_AUTH_ACCT_NOTACT:
			case RET_AUTH_ACCT_FREEZ:
			case RET_AUTH_TICKET_INVALID:
			case RET_AUTH_TICKET_EXPIRE:
			case RET_AUTH_PWD_EXPIRE:
				{
					gIcheckNume++;
				}
				break;
			case RET_AUTH_DP_ERROR:
			case RET_AUTH_SYSTEM_ERROR:
			case RET_AUTH_PROTOCOL_ERROR:
			case RET_AUTH_TIMEOUT:     //如果因为服务器没有链接上或者链接超时，kill掉进程，重新登录
				{
					OnLogin(m_strName);
					gIcheckNume++;
				}
				break;
			default:
				{
					gIcheckNume++;
				}

				break;
		}
		
		if ( gIcheckNume > 5 )
			KillTimer(nIDEvent);
	}
}

//IM消息
LRESULT CDlgIm::OnImNotify(WPARAM wParam, LPARAM lParam)
{
	CGGTongDoc* pDoc = AfxGetDocument();
	if ( NULL == pDoc || NULL == pDoc->m_pAbsCenterManager )
	{
		return 0;
	}
	
	// modify by tangad 今评功能已去掉
	//if ( 0 == (int)lParam )
	//	pDoc->m_pAbsCenterManager->SetMessageFlag(true);
	//else if ( 1 == (int)lParam )
	//	pDoc->m_pAbsCenterManager->SetMessageFlag(false);

	return 0;
}

void CDlgIm::OpenMessageWindow()
{
	if ( NULL == m_ImTrayWnd  )
	{
		CString strFileName;
		CString strConent;
		CStdioFile file;

		strFileName = m_strFullPath + _T("TrayHwnd.txt");

		if ( !file.Open(strFileName, CFile::modeRead | CFile::typeText) )
			return;

		file.ReadString(strConent);

		if ( !strConent.IsEmpty() )
		{
			int iWnd = _wtoi(strConent);
			m_iWnd = iWnd;
		
		}
	}

	m_ImTrayWnd = CWnd::FromHandle((HWND)m_iWnd);
	if ( NULL != m_ImTrayWnd && NULL != m_ImTrayWnd->m_hWnd)
		m_ImTrayWnd->SendMessage(WM_TASKTRAY_MESSAGE, 1, WM_LBUTTONDBLCLK);
}


