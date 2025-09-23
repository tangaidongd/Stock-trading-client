#include "stdafx.h"
#include "pathfactory.h"
#include "LoginUserData.h"
#include "tinyxml.h"
#include "coding.h"
#include "GridCellCheck2.h"
#include "GridCtrlNormal.h"
#include "DlgNetWorkSet.h"
#include "dlgconnectset.h"
#include "ShareFun.h"
#include "DlgNewLogin.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const int KUpdatePingTimerId = 120000;
const int KPingTimerPeriod   = 100; //每隔100毫秒


CDlgNetWorkSet::CDlgNetWorkSet(CWnd* pParent /*=NULL*/)
:CDialogEx(CDlgNetWorkSet::IDD, pParent)
{
	m_rows = 0;
	m_SelectedRow = 0;
	m_bUseProxy = false;
	m_bManual = false;
	m_isAlreadyFinsh = false;
    m_hThread = NULL;
	m_ping.m_hWnd = NULL;
}

void CDlgNetWorkSet::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgArbitrage)
	DDX_Control(pDX, IDC_GRID, m_Grid); 
	DDX_Check(pDX, IDC_CHECK_MANUAL, m_bManual);

	DDX_Control(pDX, IDC_COMBO_RUN, m_runMode);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDlgNetWorkSet, CDialogEx)
	//{{AFX_MSG_MAP(CDlgArbitrage)
	ON_CBN_SELCHANGE(IDC_COMBO_RUN, OnChangleRunMode)
	ON_WM_CREATE()
	//ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BTN_CANCEL, OnButtonCancel)
	ON_BN_CLICKED(IDC_BTN_YES, OnBtnOk)
	ON_BN_CLICKED(IDC_BTN_NETTEST, OnButtonNet)
	ON_BN_CLICKED(IDC_BTN_PROXYSERVER, OnButtonProxy)
	ON_NOTIFY(GVN_SELCHANGED, IDC_GRID, OnGridEndSelChange)
	
	ON_NOTIFY(GVL_VERT, IDC_GRID, OnGridEndSelChange)

	//ON_MESSAGE(UM_POS_CHANGED, OnScrollPosChanged)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgNetWorkSet message handlers

static int TitleCount = 5;
static wchar_t *pTitle[5] = {L" ", L"名称", L"地址", L"响应", L"速度"};
static int   nColsLength[5] = {30,85,210,60,60};


BOOL CDlgNetWorkSet::OnInitDialog()
{
	CDialog::OnInitDialog();
	m_runMode.InsertString(0, L"普通模式");
	m_runMode.InsertString(1, L"兼容模式");
	m_runMode.SetCurSel(0);
	
	FromXml();
	InitDataGrid();
	AddDataGridTitle();
	InsertRow();
	m_Grid.EnableOriginalScroll(TRUE);
	m_Grid.ShowScrollBar(SB_VERT, TRUE);

	SetCellCheckBox();

	UpdateData(FALSE);

	m_Grid.Invalidate();
	//OnButtonNet();
    m_isAlreadyFinsh = false;
	return TRUE;
}

void CDlgNetWorkSet::SetIinitDailog(CString &strName)
{
	m_StrUserName = strName;
}

CDlgNetWorkSet::~CDlgNetWorkSet()
{
	if (m_hThread)
	{
		TerminateThread(m_hThread,0);
		DEL_HANDLE(m_hThread);
	}
}

void CDlgNetWorkSet::OnBtnOk()
{
	UpdateData(TRUE);
	
	// ((CDlgLogIn *)GetParent())->SetServerInfo(m_SelectedRow - 1, m_runMode.GetCurSel(), m_bManual);
	ToXml();
	CDialog::OnOK();
};

void CDlgNetWorkSet::OnButtonCancel()
{
	CDialog::OnCancel();
}

void CDlgNetWorkSet::OnButtonNet()
{
    if (m_hThread == NULL)
    {
        unsigned int nTID = 0;
        m_hThread = (HANDLE) _beginthreadex( NULL, 0, _ThreadPingProc, this, CREATE_SUSPENDED,&nTID);

        if (!m_hThread)
        {
            TRACE(_T("Couldn't start a GIF animation thread\n"));
            return ;
        } 
        else 
            ResumeThread(m_hThread);

        m_threadStartTime = CTime::GetCurrentTime();
        GetDlgItem(IDC_BTN_NETTEST)->EnableWindow(FALSE);
        SetTimer(KUpdatePingTimerId, KPingTimerPeriod, NULL);
        m_isAlreadyFinsh = false;
    }
}

void CDlgNetWorkSet::OnButtonProxy()
{
	CDlgConnectSet dlgConnectSet;	

	T_ProxyInfo ProxyInfo;

	if (GetProxyInfoFromXml(ProxyInfo))
	{
		dlgConnectSet.SetInitialProxyInfo(ProxyInfo);
	}

	//
	dlgConnectSet.SetInitialInfo(m_StrUserName, m_bUseProxy, 0, false);
	dlgConnectSet.DoModal();	
}

void CDlgNetWorkSet::InitDataGrid()
{
	
	m_Grid.DeleteAllItems();

	m_Grid.SetRowCount(m_rows + 1); //设置行数为k行
	m_Grid.SetColumnCount(5);   //k列
	m_Grid.SetFixedRowCount(1);

	  //标题行为一行
	
	m_Grid.SetHeaderSort(TRUE);

	for (int i = 0; i <= m_rows; i++)
	{
		m_Grid.SetRowHeight(i, 12 * 2);
	}

	m_Grid.SetColumnWidth(0,  nColsLength[0]);
	m_Grid.SetColumnWidth(1,  nColsLength[1]);
	m_Grid.SetColumnWidth(2,  nColsLength[2]);
	m_Grid.SetColumnWidth(3,  nColsLength[3]);
	m_Grid.SetColumnWidth(4,  nColsLength[4]);

	m_Grid.SetListMode(TRUE);
	m_Grid.SetTrackFocusCell(FALSE);
	m_Grid.SetFrameFocusCell(FALSE);
}

void CDlgNetWorkSet::AddDataGridTitle()
{
	GV_ITEM Item;

	for (int i = 0; i < TitleCount; i++)
	{
		Item.mask = GVIF_TEXT|GVIF_FORMAT;
		Item.row = 0;
		Item.col = i;
        Item.nFormat = DT_CENTER|DT_WORDBREAK;
		 Item.strText.Format(L"%s", pTitle[i]);
		 m_Grid.SetItem(&Item);
	}
 }

void CDlgNetWorkSet::SetCellCheckBox()
{
	CGridCellCheck2 *pCellCheck = (CGridCellCheck2 *)m_Grid.GetCell( m_SelectedRow, 0);

	if ( NULL != pCellCheck )
		pCellCheck->SetCheck(TRUE);
}

void CDlgNetWorkSet::InsertRow()
{
	CString strName = "";
	GV_ITEM Item1, Item2, Item3;

	for (int i = 0; i < m_aPlusInfo.GetSize(); i++)
	{
		strName.Format(L"(%d)", m_aPlusInfo.GetAt(i).iPort);
		m_Grid.SetItemText(i + 1, 1, m_aPlusInfo.GetAt(i).strName + strName);
		m_Grid.SetItemText(i + 1, 2, m_aPlusInfo.GetAt(i).strUrl);
		m_Grid.SetCellType(i + 1,0, RUNTIME_CLASS(CGridCellCheck2));
	}
}

int CDlgNetWorkSet::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	return CDialogEx::OnCreate(lpCreateStruct);
}

bool CDlgNetWorkSet::ToXml()
{
	CString StrPath  = CPathFactory::GetServerInfoPath();
	if (StrPath.IsEmpty() )
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
	
	// 网络设置
	TiXmlElement* pNetWorkSet = pRootElement->FirstChildElement(KStrElementAttriNetWorkSet);
	if( NULL != pNetWorkSet && NULL != pNetWorkSet->Value()  )
	{
		ASSERT( 0 == strcmp(pNetWorkSet->Value(), KStrElementAttriNetWorkSet) );
		
		if ( 0 == m_runMode.GetCurSel() )
		{
				pNetWorkSet->SetAttribute(KStrElementAttriNetWorkRunMode, "0");
		}
		else
		{
				pNetWorkSet->SetAttribute(KStrElementAttriNetWorkRunMode, "1");
		}
	

		TiXmlElement* pNetWork = pNetWorkSet->FirstChildElement(KStrElementAttriNetWork);
		
		int i = 1;
		
		for ( ; NULL!=pNetWork ; pNetWork = pNetWork->NextSiblingElement(KStrElementAttriNetWork), i++ )
		{
			if ( i != (int )m_SelectedRow)
			{
				pNetWork->SetAttribute(KStrElementAttriNetWorkSelected, "0");
			}
			else
			{
				pNetWork->SetAttribute(KStrElementAttriNetWorkSelected, "1");
			}
			
		}
		
	}

	myDocument.SaveFile();
	return true;
}

bool CDlgNetWorkSet::FromXml()
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
	
	// 服务器信息
	TiXmlElement* pNetWorkSet = pRootElement->FirstChildElement(KStrElementAttriNetWorkSet);
	if( NULL != pNetWorkSet && NULL != pNetWorkSet->Value()  )
	{
		ASSERT( 0 == strcmp(pNetWorkSet->Value(), KStrElementAttriNetWorkSet) );
		
		//RunMode 
		const char *runMode = pNetWorkSet->Attribute(KStrElementAttriNetWorkRunMode);

		if (NULL != runMode)
			m_runMode.SetCurSel( atoi(runMode) );
		else
			m_runMode.SetCurSel(0);

		TiXmlElement* pNetWork = pNetWorkSet->FirstChildElement(KStrElementAttriNetWork);
		for (m_rows = 0 ; NULL!=pNetWork ; pNetWork = pNetWork->NextSiblingElement(KStrElementAttriNetWork) )
		{
			T_PlusInfo plusInfo;
			
			// 名称:
			const char* StrName = pNetWork->Attribute(KStrElementAttriNetWorkName);
			
			if ( NULL == StrName )
			{
				continue;
			}
			
			// URL:
			const char* StrUrl = pNetWork->Attribute(KStrElementAttriNetWorkURL);
			
			if ( NULL == StrUrl )
			{
				continue;
			}
			//port
			const char* strPort = pNetWork->Attribute(KStrElementAttriNetWorkPort);

			//selected 
			const char* strSelected = pNetWork->Attribute(KStrElementAttriNetWorkSelected);

			// 转换编码:
			TCHAR TStrHostName[1024];
			
			memset(TStrHostName, 0, sizeof(TStrHostName));
			MultiCharCoding2Unicode(EMCCUtf8, StrName, strlen(StrName), TStrHostName, sizeof(TStrHostName) / sizeof(TCHAR));
			plusInfo.strName = TStrHostName;

			memset(TStrHostName, 0, sizeof(TStrHostName));
			MultiCharCoding2Unicode(EMCCUtf8, StrUrl, strlen(StrUrl), TStrHostName, sizeof(TStrHostName) / sizeof(TCHAR));
			plusInfo.strUrl = TStrHostName;

			if ( NULL != strPort )
				plusInfo.iPort   = atoi(strPort);

			if (NULL != strSelected)
				plusInfo.iSelected = atoi(strSelected);
			else
				plusInfo.iSelected = 0;

			if (1 == plusInfo.iSelected)
				m_SelectedRow  = m_rows + 1;
			
			m_aPlusInfo.Add(plusInfo);

			m_rows++;
			
		}
	}

	return true;
}


bool32 CDlgNetWorkSet::GetProxyInfoFromXml(OUT T_ProxyInfo& ProxyInfo)
{
	m_bUseProxy = false;
	
	// 从私人目录下读取代理信息:
	if ( m_StrUserName.GetLength() <= 0 )
	{
		return false;
	}
	
	//
	UpdateData(true);
	
	CString StrPath = CPathFactory::GetProxyInfoPath(m_StrUserName);
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
	
	m_bUseProxy				  =  (atoi(StrUseProxy) > 0) ? true:false;
	
	return true;
}


void CDlgNetWorkSet::OnGridEndSelChange(NMHDR *pNotifyStruct, LRESULT* pResult)
{
	NM_GRIDVIEW* pItem = (NM_GRIDVIEW*) pNotifyStruct; 
	if (pItem->iRow > 0)
	{
		CGridCellCheck2 *pCellCheck = (CGridCellCheck2 *)m_Grid.GetCell( (int)m_SelectedRow , 0);
		if ( NULL != pCellCheck )
			pCellCheck->SetCheck(FALSE);

		pCellCheck = (CGridCellCheck2 *)m_Grid.GetCell(pItem->iRow, 0);

		if ( NULL != pCellCheck )
			pCellCheck->SetCheck(TRUE);

		m_SelectedRow = pItem->iRow;
	//	OnPing(m_SelectedRow - 1);
	}
	UpdateData(FALSE);
}

bool CDlgNetWorkSet::DosPing(LPCSTR szTarget, int &nTime)
{
	BOOL bSuccess = FALSE; 
    nTime = -1; 
 
    if(szTarget == NULL) 
    { 
        return FALSE; 
    } 

	CString strIP = szTarget;
 
    tchar szCmdLine[80];
    if(_sntprintf(szCmdLine, sizeof(szCmdLine) / sizeof(TCHAR), 
        L"ping.exe -n 1 %s", strIP.GetBuffer()) == sizeof(szCmdLine) / sizeof(TCHAR)) 
    {  
        return FALSE;
    } 
 
    HANDLE hWritePipe = NULL; 
    HANDLE hReadPipe = NULL; 
    HANDLE hWriteShell = NULL; 
    HANDLE hReadShell = NULL; 
 
    SECURITY_ATTRIBUTES  sa; 
    memset(&sa, 0, sizeof(sa)); 
    sa.nLength = sizeof(sa); 
    sa.bInheritHandle = TRUE; 
    sa.lpSecurityDescriptor = NULL; 
 
    if(CreatePipe(&hReadPipe, &hReadShell, &sa, 0) 
        && CreatePipe(&hWriteShell, &hWritePipe, &sa, 0)) 
    { 
        STARTUPINFO            si; 
        memset(&si, 0, sizeof(si)); 
        si.cb           = sizeof(si); 
        si.dwFlags      = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES; 
        si.hStdInput    = hWriteShell; 
        si.hStdOutput   = hReadShell; 
        si.hStdError    = hReadShell; 
        si.wShowWindow  = SW_HIDE; 
         
        PROCESS_INFORMATION    pi; 
        memset(&pi, 0, sizeof(pi)); 
         
        int nMin = -1, nMax = -1, nAvg = -1; 
        if(CreateProcess(NULL, szCmdLine, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi)) 
        { 
            if(WaitForSingleObject(pi.hProcess, 10000) == WAIT_OBJECT_0) 
            { 
				char  sBuffer[1024];
                DWORD dwBytes; 
                if(ReadFile(hReadPipe, sBuffer, sizeof(sBuffer), &dwBytes, NULL)) 
                { 
					CString strTmp = sBuffer;
 
                    LPCTSTR lpszTime = NULL; 
					LPCTSTR lpszTime2 = NULL;
                    lpszTime = _tcsstr(strTmp, L"请求超时。"); 
					lpszTime2 = _tcsstr(strTmp, L"Request timed out");
                    if(lpszTime == NULL && lpszTime2 ==NULL) 
                    { 
                        lpszTime = _tcsstr(strTmp, L"最短"); 
                        if(lpszTime != NULL) 
                        { 
                            if(_stscanf(lpszTime, L"最短 = %dms，最长 = %dms，平均 = %dms", 
                                &nMin, &nMax, &nAvg) == 3) 
                            { 
                                TRACE3("%d, %d, %d\n", nMin, nMax, nAvg); 
 
                                nTime = nAvg; 
                                bSuccess = TRUE; 
                            } 
                        } 
                        else 
                        { 
							lpszTime = _tcsstr(strTmp, L"Minimum"); 
							if(lpszTime != NULL) 
							{ 
								if(_stscanf(lpszTime, L"Minimum = %dms, Maximum = %dms, Average = %dms", 
                                &nMin, &nMax, &nAvg) == 3) 
								{ 
									TRACE3("%d, %d, %d\n", nMin, nMax, nAvg); 
									
									nTime = nAvg; 
									bSuccess = TRUE; 
								} 
							} 
							else
							{
								TRACE0("PING FORMAT is Error\n"); 
							}
                        } 
                    } 
                    else 
                    { 
                        TRACE0("PING is Time Out\n"); 
						nTime = -2;
                        bSuccess = TRUE; 
                    } 
                } 
            } 
            else 
            { 
                TRACE1("Process(%d) is Time Out\n", pi.dwProcessId); 
                TerminateProcess(pi.hProcess, 0); 
            } 
 
            DEL_HANDLE(pi.hThread); 
            DEL_HANDLE(pi.hProcess); 
 
        //    TRACE3(_T("Minimum = %dms, Maximum = %dms, Average = %dms\n"), nMin, nMax, nAvg); 
        } 
    } 
 
    if(hWritePipe != NULL) 
        DEL_HANDLE(hWritePipe); 
    if(hReadPipe != NULL) 
        DEL_HANDLE(hReadPipe); 
    if(hWriteShell != NULL) 
        DEL_HANDLE(hWriteShell); 
    if(hReadShell != NULL) 
        DEL_HANDLE(hReadShell); 
 
    return bSuccess; 
}

void CDlgNetWorkSet::OnPing(int nRow)
{
	char tmpBuf[1024];
	PINGINFO pinpInfo;
	CString strTime;
	CString strSpeed;
	
	int nRet = -1;
	
	Unicode2MultiCharCoding( EMCCUtf8, m_aPlusInfo.GetAt(nRow).strUrl, -1, tmpBuf, sizeof(tmpBuf) );
	nRet = m_ping.Ping(1, tmpBuf, this->m_hWnd, (char *)&pinpInfo);

	int nTime = -1;
	if (-1 == nRet)
	{
		if(DosPing(tmpBuf, nTime))
		{
			nRet = 0;
			pinpInfo.dwTime = nTime;
		}
		else
		{
			nRet = nTime;
		}
	}
	
	if ( 0 == nRet)
	{
		if ( pinpInfo.dwTime < 50)
		{
			strSpeed = L"较快";
		}
		else if ( pinpInfo.dwTime < 100)
		{
			strSpeed = L"快";
		}
		else if ( pinpInfo.dwTime  < 150)
		{
			strSpeed = L"慢";
		}
		else 
		{
			strSpeed = L"较慢";
		}
		
		strTime.Format(L"%ld ms", pinpInfo.dwTime);
	}
	else
	{
		strTime = L"";
	}
	
	if ( -1 == nRet)
	{
		strSpeed =L"链接失败";
	}
	
	if (-2 == nRet)
	{
		strSpeed = L"链接超时";
	}
	m_Grid.SetItemText(nRow + 1, 3, strTime);
	m_Grid.SetItemText(nRow + 1, 4, strSpeed);

	m_Grid.Invalidate();
}

void CDlgNetWorkSet::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);
	// TODO: Add your message handler code here
	if ( m_Grid.GetSafeHwnd() )
	{
		m_Grid.ShowWindow(SW_SHOW);
		// 设置位置:
		CRect rectGrid;
		GetDlgItem(IDC_GRID)->GetWindowRect(&rectGrid);
		ScreenToClient(&rectGrid);	
		m_Grid.MoveWindow(&rectGrid);
		m_Grid.ShowWindow(SW_SHOW | SW_SHOWNOACTIVATE);
		
		// 滚动条
		CRect RectSB(rectGrid);
		RectSB.left = rectGrid.right;
		RectSB.right= RectSB.left + 1;
		m_XSBVert.SetSBRect(RectSB, FALSE);
		
		CRect RectSH(rectGrid);
		RectSH.top = rectGrid.bottom;
		RectSH.bottom= RectSH.top + 20;
		m_XSBHert.SetSBRect(RectSH, FALSE);
	
	}
}

void CDlgNetWorkSet::OnChangleRunMode()
{
	UpdateData(TRUE);
}

void CDlgNetWorkSet::OnTimer(UINT nIDEvent)
{
	if ( KUpdatePingTimerId  == nIDEvent )
	{
		//OnButtonNet();
        if((CTime::GetCurrentTime() - m_threadStartTime > 5000) || m_isAlreadyFinsh)
        {
            DestroyThread();
            GetDlgItem(IDC_BTN_NETTEST)->EnableWindow();
            KillTimer(KUpdatePingTimerId);
        }
	}
}

int CDlgNetWorkSet::GetNetSelectIndex()
{
	if ( 0 != m_SelectedRow )
		return m_SelectedRow - 1;

	return 0;
}

UINT WINAPI CDlgNetWorkSet::_ThreadPingProc( LPVOID pParam )
{
    ASSERT(pParam);
    CDlgNetWorkSet *pDlg = (CDlgNetWorkSet*)pParam;
    for (int i = 0; i < pDlg->m_rows; i++)
        pDlg->OnPing(i);

    pDlg->m_isAlreadyFinsh = true;
    return 0;
}

void CDlgNetWorkSet::DestroyThread()
{
    if (m_hThread)
    {
        WaitForSingleObject(m_hThread, INFINITE);
    }

    DEL_HANDLE(m_hThread);
}
