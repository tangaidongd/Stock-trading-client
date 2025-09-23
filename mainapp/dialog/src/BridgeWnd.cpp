#include "StdAfx.h"
#include "BridgeWnd.h"
#include "PathFactory.h"
#include "coding.h"
#include "LoginUserData.h"
#define  TRACE_ELAPSE_BEGIN() iTickBegin = GetTickCount();
#define  TRACE_ELAPSE_END(msg)\
{\
	int iTickEnd = GetTickCount();\
	float f = (iTickEnd - iTickBegin) / 1000.0f;\
	TRACE("%s:%f\n", msg, f);\
}
#define  TRACE_ELAPSE_END_TWO(iTickBegin, msg)\
{\
	int iTickEnd = GetTickCount();\
	float f = (iTickEnd - iTickBegin) / 1000.0f;\
	TRACE("%s:%f\n", msg, f);\
}
//	��ȡ���������
extern void GetServerInfoFromXml(CArray<T_HostInfo, T_HostInfo>& arrHostInfo);

namespace
{
#define WM_EXIT_MESSAGE WM_USER+1
}
CBridgeWnd::CBridgeWnd(void)
{
	m_hWnd = NULL;	
}

CBridgeWnd::~CBridgeWnd(void)
{
	m_hWnd = NULL;
	if (!m_tClassName.IsEmpty())
	{
		UnregisterClass(m_tClassName, AfxGetApp()->m_hInstance);
	}	
}

HWND	CBridgeWnd::Create()
{
	try
	{
		m_tClassName=AfxRegisterWndClass(CS_DBLCLKS, AfxGetApp()->LoadStandardCursor(IDC_ARROW), (HBRUSH)(COLOR_3DFACE+1), NULL);
		m_hWnd=CreateWindow(m_tClassName, NULL, 0, 0, 0, 0, 0, HWND_MESSAGE, NULL, AfxGetApp()->m_hInstance, NULL);	
		ASSERT(m_hWnd);
		::SetWindowLong(m_hWnd, GWL_WNDPROC, (LONG)CBridgeWnd::WindowProc);
		::SetTimer(m_hWnd, 1, 10000, NULL);
	}
	catch (CResourceException* pEx)
	{
		AfxMessageBox(_T("Couldn't register class! (Already registered?)"));
		pEx->Delete();
	}		
	return m_hWnd;
}


LRESULT CALLBACK CBridgeWnd::WindowProc(__in HWND hWnd, __in UINT Msg,  __in WPARAM wParam,  __in LPARAM lParam)
{
	switch(Msg)
	{
	case UM_BridgeGetNetBreedFailed:
		if (0 == wParam)
		{
			//	δ֪ԭ�����Ȩ���������ش��г���Ϣʧ��
				::PostMessage(hWnd, WM_EXIT_MESSAGE, 1, 0);
		}
		break;
	case WM_TIMER:
		if (1 == wParam)
		{
			//	̫��û�յ��г���Ϣ�ˣ�̫���ˡ����ˡ�						
			::PostMessage(hWnd, WM_EXIT_MESSAGE, 1, 0);
			return -1;
		}
		break;
	case UM_BridgeBegin:
		{
			//	�Ѿ��յ��г���Ϣ�ˣ����ڴ����У����Է�����			
			KillTimer(hWnd, 1);
		}
		break;
	case UM_BridgeEnd:
		{		
			//	�Ѿ�����֤�������Ϸ������г��ˣ����Է��ĵ�¼��			
			::PostMessage(hWnd, WM_EXIT_MESSAGE, 0, 0);
			return 0;
		}
	case WM_EXIT_MESSAGE:
		return wParam;
		break;		
	}
	return ::DefWindowProc(hWnd, Msg, wParam, lParam);
}

LRESULT CBridgeWnd::Loop()
{
	ASSERT(NULL != m_hWnd);	
	MSG msg;
	BOOL bRet = 0; 
	while( (bRet = GetMessage( &msg, m_hWnd, 0, 0 )) != 0)
	{ 
		
		if (bRet == -1)
		{
			// handle the error and possibly exit
			break;
		}
		else
		{
			TranslateMessage(&msg); 
			DispatchMessage(&msg); 
		}
		//������ɣ�׼���˳�
		if (msg.message == WM_EXIT_MESSAGE)
		{		
			return msg.wParam;
		}
	} 

	return bRet;
}
//	��ȡ���������
void GetServerInfoFromXml(CArray<T_HostInfo, T_HostInfo>& arrHostInfo)
{
	// �ӹ�����config Ŀ¼�� Xml �ļ��л�÷�������Ϣ:

	// 1: ��ձ�������:
	arrHostInfo.RemoveAll();

	// 2: ���ļ�:
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

	// ServerInfoQuote, �����������Ϣ
	TiXmlElement* pQuoteServersElement = pRootElement->FirstChildElement(KStrElementNameServerInfoQuote);
	if( NULL != pQuoteServersElement && NULL != pQuoteServersElement->Value()  )
	{
		ASSERT( 0 == strcmp(pQuoteServersElement->Value(), KStrElementNameServerInfoQuote) );
		TiXmlElement* pServerElement = pQuoteServersElement->FirstChildElement();

		while(pServerElement)
		{	
			// ��������Server�ڵ�:
			if ( NULL == pServerElement->Value() || 0 != strcmp(pServerElement->Value(), KStrElementNameServer))
			{
				pServerElement = pServerElement->NextSiblingElement();
				continue;
			}

			T_HostInfo ServerInfo;

			// ����:
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

			// �˿�:
			const char* StrPort = pServerElement->Attribute(KStrElementAttriServerPort);

			if ( NULL == StrPort )
			{
				pServerElement = pServerElement->NextSiblingElement();
				continue;
			} 

			// ��������:
			UINT uiNetType = 1;

			const char* StrNetType = pServerElement->Attribute(KStrElementAttriServerNetType);
			if ( NULL != StrNetType )
			{
				uiNetType = atoi(StrNetType);				
			}

			// ת������:
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

			arrHostInfo.Add(ServerInfo);

			// ������һ��:
			pServerElement = pServerElement->NextSiblingElement();
		}		
	}
}
//	���ߵ�¼
bool OfflineLogin()
{	
	int iTickBegin = GetTickCount();
	CGGTongApp *pApp  =  (CGGTongApp*) AfxGetApp();
	if(NULL == pApp || NULL == pApp->m_pDocument || NULL == pApp->m_pDocument)
	{
		ASSERT(0);
		return false;
	}


	CGGTongDoc *pDoc = pApp->m_pDocument;
	if(!pDoc->OnLoginDlgDoModalSuccess())
	{
		ASSERT(0);
		return false;
	}	


	//	����һ����Ϣ���壬�������ջ�����Ϣ
	CBridgeWnd bridge;
	HWND hWnd = bridge.Create();


	CArray<T_HostInfo, T_HostInfo> arrHostInfo;
	GetServerInfoFromXml(arrHostInfo);
	if(0 == arrHostInfo.GetSize())
	{
		ASSERT(0);
		MessageBox(NULL, L"��������������ļ�����", L"����", MB_OK|MB_ICONWARNING);
		return false;
	}

	pDoc->m_pAbsCenterManager->SetBridge(hWnd);
	//	���ߵ�¼
	if ( !pDoc->m_pAbsCenterManager->OfflineLogin(CConfigInfo::Instance()->m_StrDefUser, arrHostInfo[0].StrHostAddress, CConfigInfo::Instance()->GetAuthKey(), CConfigInfo::Instance()->GetOrgKey()) )
	{		
		MessageBox(NULL, L"���ߵ�¼ʧ��", L"���ߵ�¼", MB_OK|MB_ICONWARNING);
		return false;
	}

	bool bSuc = false;
	LRESULT rt = bridge.Loop();
	switch (rt)
	{
	case 0:
		//	��֤�г��ɹ�
		bSuc = true;
		break;
	case 1:
		//	��֤�г���ʱ	
		bSuc = false;
	default:
		break;
	}

	
	return bSuc;
}
