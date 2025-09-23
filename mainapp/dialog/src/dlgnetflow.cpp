// dlgnetflow.cpp : implementation file
//

#include "stdafx.h"



#include "dlgnetflow.h"
#include "NewsEngineManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
static const int KiTimerIDRefresh	  = 1;
static const int KiTimerPeriodRefresh = 100;

/////////////////////////////////////////////////////////////////////////////
// CDlgNetFlow dialog

CDlgNetFlow::CDlgNetFlow(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgNetFlow::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgNetFlow)
	m_StrBegin = _T("");
	m_StrEnd = _T("");
	//}}AFX_DATA_INIT

	m_bPaused = false;
	
	//
	m_mapReference.clear();
	m_mapCurrent.clear();
}

void CDlgNetFlow::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgNetFlow)
	DDX_Control(pDX, IDC_LIST_NOW, m_ListNow);
	DDX_Control(pDX, IDC_LIST_HISTORY, m_ListHistory);
	DDX_Text(pDX, IDC_STATIC_BEGIN, m_StrBegin);
	DDX_Text(pDX, IDC_STATIC_END, m_StrEnd);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgNetFlow, CDialogEx)
	//{{AFX_MSG_MAP(CDlgNetFlow)
	ON_BN_CLICKED(IDC_BUTTON_PAUSE, OnButtonPause)
	ON_BN_CLICKED(IDC_BUTTON_RESET, OnButtonReset)
	ON_WM_TIMER()
	ON_WM_CLOSE()
	ON_BN_CLICKED(ID_HIDE, OnHide)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgNetFlow message handlers
BOOL CDlgNetFlow::PreTranslateMessage(MSG* pMsg)
{
	if ( WM_KEYDOWN == pMsg->message )
	{
		if ( VK_RETURN == pMsg->wParam || VK_ESCAPE == pMsg->wParam )
		{
			OnClose();
			return TRUE;
		}
	}

	return FALSE;
}

BOOL CDlgNetFlow::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	CenterWindow();

	//
	SetCurrentValue();
	SetReferenceValue();
	InitialControls();

	//
	SetTimer(KiTimerIDRefresh, KiTimerPeriodRefresh, NULL);

	return TRUE;
}

void CDlgNetFlow::OnClose()
{
	//
	KillTimer(KiTimerIDRefresh);

	CMainFrame* pWnd = (CMainFrame*)AfxGetMainWnd();
	pWnd->m_pDlgNetFlow = NULL;	
	
	DestroyWindow();
}

void CDlgNetFlow::PostNcDestroy()
{
	CDialog::PostNcDestroy();
	delete this;
}

void CDlgNetFlow::OnServerReConnected(int32 iCommunicationID)
{
	// 某台服务器断线了, 将其数据清空
	for ( mapServerNetFlow::iterator it = m_mapReference.begin(); it != m_mapReference.end(); ++it )
	{
		if ( (it->first.m_bQuoteServer) && (iCommunicationID == it->first.m_iCommunicationID) )
		{
			it->second.m_uiRecvBytes = 0;
			it->second.m_uiSendBytes = 0;	
		}
	}

	for ( mapServerNetFlow::iterator it2 = m_mapCurrent.begin(); it2 != m_mapCurrent.end(); ++it2 )
	{
		if ( (it2->first.m_bQuoteServer) && iCommunicationID == it2->first.m_iCommunicationID )
		{								
			it2->second.m_uiTimeStarted = time(NULL);	
		}
	}
}

void CDlgNetFlow::OnNewsServerReConnected(int32 iCommunicationID)
{
	// 某台服务器断线了, 将其数据清空
	for ( mapServerNetFlow::iterator it = m_mapReference.begin(); it != m_mapReference.end(); ++it )
	{
		if ( (!it->first.m_bQuoteServer) && iCommunicationID == it->first.m_iCommunicationID )
		{
			it->second.m_uiRecvBytes = 0;
			it->second.m_uiSendBytes = 0;			
		}
	}

	for ( mapServerNetFlow::iterator it2 = m_mapCurrent.begin(); it2 != m_mapCurrent.end(); ++it2 )
	{
		if ( (!it2->first.m_bQuoteServer) && iCommunicationID == it2->first.m_iCommunicationID )
		{					
			it2->second.m_uiTimeStarted = time(NULL);			
		}
	}
}

void CDlgNetFlow::Test()
{
	for ( mapServerNetFlow::iterator it = m_mapReference.begin(); it != m_mapReference.end(); ++it )
	{
		T_ServerKey   tkey 	 = it->first;
		T_ServerValue tvalue = it->second;

		TRACE(L"mapRef: 【%d】 ID:%d Name:%s Add:%s:%d Recv:%d Send:%d TimeStart: %d\n\n", 
				tkey.m_bQuoteServer, tkey.m_iCommunicationID, tkey.m_StrServerName.GetBuffer(), tkey.m_StrServerAddr.GetBuffer(), tkey.m_uiPort, 
				tvalue.m_uiRecvBytes, tvalue.m_uiSendBytes, tvalue.m_uiTimeStarted);
		
	}	
	
	{
		for ( mapServerNetFlow::iterator it = m_mapCurrent.begin(); it != m_mapCurrent.end(); ++it )
		{
			T_ServerKey   tkey 	 = it->first;
			T_ServerValue tvalue = it->second;
			
			TRACE(L"mapNow: 【%d】 ID:%d Name:%s Add:%s:%d Recv:%d Send:%d TimeStart: %d\n\n", 
					tkey.m_bQuoteServer, tkey.m_iCommunicationID, tkey.m_StrServerName.GetBuffer(), tkey.m_StrServerAddr.GetBuffer(), tkey.m_uiPort, 
					tvalue.m_uiRecvBytes, tvalue.m_uiSendBytes, tvalue.m_uiTimeStarted);
				
		}
	}
}

void CDlgNetFlow::InitialControls()
{	
	// 历史流量
	{
		m_ListHistory.InsertColumn(0, L"名称");
		m_ListHistory.InsertColumn(1, L"地址");
		m_ListHistory.InsertColumn(2, L"上行");
		m_ListHistory.InsertColumn(3, L"下行");
		
		m_ListHistory.SetColumnWidth(0, 60);
		m_ListHistory.SetColumnWidth(1, 150);
		m_ListHistory.SetColumnWidth(2, 127);
		m_ListHistory.SetColumnWidth(3, 127);

		int iIndex = 0;

		//
		for ( mapServerNetFlow::iterator it = m_mapReference.begin(); it != m_mapReference.end(); ++it )
		{
			// 名称
			CString StrName;
			StrName = it->first.m_StrServerName;
			
			// 地址
			CString StrAdd;
			CString StrAddr = it->first.m_StrServerAddr;
			StrAdd.Format(L"%s:%d", StrAddr.GetBuffer(), it->first.m_uiPort);
			
			m_ListHistory.InsertItem(iIndex, StrName);
			m_ListHistory.SetItemText(iIndex, 1, StrAdd);
			
			// 设置自定义数据
			m_ListHistory.SetItemData(iIndex, (DWORD)&it->first);
					
			//
			++iIndex;
		}
	}

	// 统计流量
	{
		m_ListNow.InsertColumn(0, L"名称");
		m_ListNow.InsertColumn(1, L"地址");
		m_ListNow.InsertColumn(2, L"上行");
		m_ListNow.InsertColumn(3, L"下行");
		
		m_ListNow.SetColumnWidth(0, 60);
		m_ListNow.SetColumnWidth(1, 150);
		m_ListNow.SetColumnWidth(2, 127);
		m_ListNow.SetColumnWidth(3, 127);

		int iIndex = 0;
		
		//
		for ( mapServerNetFlow::iterator it = m_mapReference.begin(); it != m_mapReference.end(); ++it )
		{
			// 名称
			CString StrName;
			StrName = it->first.m_StrServerName;
			
			// 地址
			CString StrAdd;
			CString StrAddr = it->first.m_StrServerAddr;
			StrAdd.Format(L"%s:%d", StrAddr.GetBuffer(), it->first.m_uiPort);
			
			m_ListNow.InsertItem(iIndex, StrName);
			m_ListNow.SetItemText(iIndex, 1, StrAdd);
			
			// 设置自定义数据
			m_ListNow.SetItemData(iIndex, (DWORD)&it->first);

			//
			++iIndex;
		}
	}

	// 时间:
	m_StrBegin = GetTimeString(time(NULL));
	m_StrEnd   = GetTimeString(time(NULL));	
}

void CDlgNetFlow::UpdateControls()
{
	// 
	for ( mapServerNetFlow::iterator itReference = m_mapReference.begin(); itReference != m_mapReference.end(); ++itReference )
	{
		const T_ServerKey&	stServerKey	   = itReference->first;

		//
		CString StrUpLoad1;
		CString StrDownLoad1;

		CString StrUpLoad2;
		CString StrDownLoad2;

		//		
		mapServerNetFlow::iterator itNow = m_mapCurrent.find(stServerKey);
	
		if ( m_mapCurrent.end() != itNow )
		{
			// 取行号
			int32 iIndexHistory = GetItemByUserData(&m_ListHistory, &stServerKey);
			if ( iIndexHistory >= 0 )
			{
				// 历史流量:
				float fSpeedUpload1		= 0.;
				float fSpeedDownLoad1	= 0.;

				if ( 0 != itNow->second.m_uiKeepSeconds )
				{
					fSpeedUpload1	= (float)itNow->second.m_uiSendBytes / (itNow->second.m_uiKeepSeconds * 1024);
					fSpeedDownLoad1 = (float)itNow->second.m_uiRecvBytes / (itNow->second.m_uiKeepSeconds * 1024);
				}

				StrUpLoad1.Format(L"%dB (%.2fKB/S)", itNow->second.m_uiSendBytes, fSpeedUpload1);			
				StrDownLoad1.Format(L"%dB (%.2fKB/S)", itNow->second.m_uiRecvBytes, fSpeedDownLoad1);

				//
				m_ListHistory.SetItemText(iIndexHistory, 2, StrUpLoad1);
				m_ListHistory.SetItemText(iIndexHistory, 3, StrDownLoad1);	
			}

			if ( !m_bPaused )
			{
				int32 iIndexNow = GetItemByUserData(&m_ListNow, &stServerKey);

				if ( iIndexNow >= 0 )
				{
					// 统计流量
					float fSpeedUpload2		= 0.;
					float fSpeedDownLoad2	= 0.;
					
					uint32 uUpload		= itNow->second.m_uiSendBytes - itReference->second.m_uiSendBytes;
					uint32 uDownLoad	= itNow->second.m_uiRecvBytes - itReference->second.m_uiRecvBytes;

					uint32 uiNow	= time(NULL);
					uint32 uiStart	= itNow->second.m_uiTimeStarted;

					float fTime = uiNow - uiStart;
					
					if ( 0. != fTime && itNow->second.m_uiKeepSeconds > 0 )
					{
						fSpeedUpload2	= (float)uUpload / (fTime * 1024);
						fSpeedDownLoad2 = (float)uDownLoad / (fTime * 1024);
					}

					StrUpLoad2.Format(L"%dB (%.2fKB/S)", uUpload, fSpeedUpload2);			
					StrDownLoad2.Format(L"%dB (%.2fKB/S)", uDownLoad, fSpeedDownLoad2);
					
					// 
					m_ListNow.SetItemText(iIndexNow, 2, StrUpLoad2);
					m_ListNow.SetItemText(iIndexNow, 3, StrDownLoad2);	
				}				
			}			
		}
	}

	if ( !m_bPaused )
	{
		m_StrEnd = GetTimeString(time(NULL));
	}

	UpdateData(FALSE);
}

CString CDlgNetFlow::GetTimeString(time_t timeSource)
{
	struct tm* pstTimeTmp = localtime(&timeSource);
	
	if ( NULL == pstTimeTmp)
	{
		return L"";
	}

	//
	CString StrTime;
	StrTime.Format(L"%04d-%02d-%02d %02d:%02d:%02d", pstTimeTmp->tm_year + 1900, pstTimeTmp->tm_mon+1, pstTimeTmp->tm_mday, pstTimeTmp->tm_hour, pstTimeTmp->tm_min, pstTimeTmp->tm_sec);

	return StrTime;
}

void CDlgNetFlow::SetReferenceValue()
{
	//	
	m_mapReference = m_mapCurrent;

	//
	int32 iIndex = 0;
	
	for ( mapServerNetFlow::iterator itReference = m_mapReference.begin(); itReference != m_mapReference.end(); ++itReference )
	{
		// 设置自定义数据
		m_ListHistory.SetItemData(iIndex, (DWORD)&itReference->first);
		m_ListNow.SetItemData(iIndex, (DWORD)&itReference->first);

		//
		++iIndex;
	}

}

void CDlgNetFlow::SetCurrentValue()
{
	//
	CGGTongDoc* pDoc = (CGGTongDoc*)AfxGetDocument();
	if ( NULL == pDoc )
	{
		return;
	}
	
	// 行情服务器:
	if ( NULL != pDoc->m_pAbsDataManager )
	{
		CArray<CAbsCommunicationShow*, CAbsCommunicationShow*> aCommunicationList;
		pDoc->m_pAbsDataManager->GetCommnunicaionList(aCommunicationList);
		
		for ( int32 i = 0; i < aCommunicationList.GetSize(); i++ )
		{
			CAbsCommunicationShow* pCommication = aCommunicationList[i];
			if ( NULL == pCommication )
			{
				continue;
			}
			
			//			
			CServerState ServerState;			
			pCommication->GetServerState(ServerState);
			
			T_ServerKey stServerKey;
			stServerKey.m_bQuoteServer		= true;
			stServerKey.m_iCommunicationID	= ServerState.m_iCommunicationID;
			stServerKey.m_StrServerAddr		= ServerState.m_StrServerAddr;
			stServerKey.m_uiPort			= ServerState.m_uiServerPort;
			stServerKey.m_StrServerName		= ServerState.m_StrServerName;
			
			//
			T_ServerValue stServerValue;
			stServerValue.m_uiRecvBytes		= ServerState.m_uiRecvBytes;
			stServerValue.m_uiSendBytes		= ServerState.m_uiSendBytes;
			stServerValue.m_uiKeepSeconds	= ServerState.m_uiKeepSecond;

			mapServerNetFlow::iterator itFind = m_mapCurrent.find(stServerKey);
			if ( m_mapCurrent.end() != itFind )
			{
				stServerValue.m_uiTimeStarted = itFind->second.m_uiTimeStarted;
			}
			else
			{
				stServerValue.m_uiTimeStarted = time(NULL);
			}

			//
			m_mapCurrent[stServerKey] = stServerValue;
		}	
	}
	
	// 资讯服务器:
	if ( NULL != pDoc->m_pNewsManager )
	{			   
		CArray<CAbsNewsCommnunicationShow*, CAbsNewsCommnunicationShow*> aCommunicationList;
		pDoc->m_pNewsManager->GetCommnunicaionList(aCommunicationList);
		
		for ( int32 i = 0; i < aCommunicationList.GetSize(); i++ )
		{
			CAbsNewsCommnunicationShow* pCommication = aCommunicationList[i];
			if ( NULL == pCommication )
			{
				continue;
			}
			
			CNewsServerState ServerState;
			pCommication->GetNewsServerState(ServerState);
			
			T_ServerKey stServerKey;
			stServerKey.m_bQuoteServer		= false;
			stServerKey.m_iCommunicationID	= ServerState.m_iCommunicationID;
			stServerKey.m_StrServerAddr		= ServerState.m_StrServerAddr;
			stServerKey.m_uiPort			= ServerState.m_uiServerPort;
			stServerKey.m_StrServerName		= ServerState.m_StrServerName;
			
			T_ServerValue stServerValue;
			stServerValue.m_uiRecvBytes		= ServerState.m_uiRecvBytes;
			stServerValue.m_uiSendBytes		= ServerState.m_uiSendBytes;
			stServerValue.m_uiKeepSeconds	= ServerState.m_uiKeepSecond;

			//
			mapServerNetFlow::iterator itFind = m_mapCurrent.find(stServerKey);
			if ( m_mapCurrent.end() != itFind )
			{
				stServerValue.m_uiTimeStarted = itFind->second.m_uiTimeStarted;
			}
			else
			{
				stServerValue.m_uiTimeStarted = time(NULL);
			}
	
			//
			m_mapCurrent[stServerKey] = stServerValue;
		}
	}
}

int32 CDlgNetFlow::GetItemByUserData(const CListCtrl* pList, const T_ServerKey* pData)
{
	if ( NULL == pList || NULL == pData )
	{
		return -1;
	}

	//
	for ( int32 i = 0; i < pList->GetItemCount(); i++ )
	{
		T_ServerKey* pItemData = (T_ServerKey*)pList->GetItemData(i);

		if ( NULL == pItemData )
		{
			continue;
		}

		//
		if ( *pItemData == *pData )
		{
			return i;
		}
	}

	return -1;
}

void CDlgNetFlow::OnButtonPause() 
{
	m_bPaused = !m_bPaused;

	if ( m_bPaused )
	{
		GetDlgItem(IDC_BUTTON_PAUSE)->SetWindowText(L"继续");
	}
	else
	{
		GetDlgItem(IDC_BUTTON_PAUSE)->SetWindowText(L"暂停");
	}
}

void CDlgNetFlow::OnButtonReset() 
{
	if ( m_bPaused )
	{
		OnButtonPause();
	}

	SetReferenceValue();

	uint32 uiTimeStart = time(NULL);
	m_StrBegin = GetTimeString(uiTimeStart);

	//
	for ( mapServerNetFlow::iterator it = m_mapCurrent.begin(); it != m_mapCurrent.end(); ++it )
	{
		it->second.m_uiTimeStarted = uiTimeStart;
	}

	UpdateData(FALSE);
}

void CDlgNetFlow::OnHide() 
{
	ShowWindow(SW_HIDE);	
}

void CDlgNetFlow::OnTimer(UINT nIDEvent)
{
	if ( nIDEvent == KiTimerIDRefresh )
	{
		//
		SetCurrentValue();
		
		//
		UpdateControls();
	}
}
