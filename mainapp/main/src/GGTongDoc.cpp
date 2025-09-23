// GGTongDoc.cpp : implementation of the CGGTongDoc class
//

#include "stdafx.h"

#include "ShareFun.h"
#include "GmtTime.h"

//#include "NewsEngineManager.h"
#include "SocketClient.h"

#include "DlgNewLogin.h"
#include "SocketClient.h"

#include "dlgbuding.h"
#include "proxy_auth_client_base.h"
#include "ProxyAuthClientHelper.h"
#include "IoViewManager.h"
#include "AlarmCenter.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGGTongDoc

class CForceDeleteDoc
{
public:
	CForceDeleteDoc()
	{
		m_pDoc = NULL;
	}
	/*
		Warning -- Pointer member 'CForceDeleteDoc::m_pDoc' (line 49) neither freed nor zeroed by destructor -- Effective C++ #6
	*/
	//lint --e{1540}
	~CForceDeleteDoc()
	{
// ...fangz0722
//		if ( NULL != m_pDoc && NULL != m_pDoc->m_pAuthManager )
//		{
//			m_pDoc->m_pAuthManager->StopCommunication();
//		}
	}
	CGGTongDoc* m_pDoc;
};

static CForceDeleteDoc SForceDeleteDoc;

IMPLEMENT_DYNCREATE(CGGTongDoc, CDocument)

BEGIN_MESSAGE_MAP(CGGTongDoc, CDocument)
	//{{AFX_MSG_MAP(CGGTongDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGGTongDoc construction/destruction

//CFaceScheme CGGTongDoc::m_FaceScheme;

CGGTongDoc::CGGTongDoc()
:m_pAbsCenterManager(NULL),
m_pAutherManager(NULL),
m_pDlgLogIn(NULL),
m_pDlgbuding(NULL),
m_pAarmCneter(NULL),
m_pHttpClient(NULL)
{
	SForceDeleteDoc.m_pDoc = this;
	// TODO: add one-time construction code here
	m_bNewUser = false;		// 默认为非第一次登录用户
	m_eNetType = ENTOther;

	m_bAutoLogin = FALSE;
	m_bAutoRun   = FALSE;
	m_bShowSCToolBar = FALSE;
	
    m_StrInfoCenterIP = L"";
	m_nInfoCenterPort = 0;
	m_strEconoServerURL = "";
	m_StrOpenId =  L"";
	m_StrLoginType = L"";

	m_StrGateWayIP = L"";
	m_nGateWayPort = 0;
	m_StrPickModelIP = L"";
	m_nPickModelPort = 0;
	m_StrQueryStockIP = L"";
	m_nQueryStockPort = 0;


	m_isDefaultUser = true;
	m_bReqEcono = false;
	m_bReqMessageInfo = false;
}

//lint --e{1579}
CGGTongDoc::~CGGTongDoc()
{
	
	SForceDeleteDoc.m_pDoc = NULL;

	if (NULL != m_pDlgbuding)
	{
		m_pDlgbuding->DestroyWindow();
		DEL(m_pDlgbuding);
	}

	if (m_pAarmCneter != NULL)
	{
		DEL(m_pAarmCneter);
	}

	DestroyAllGGTongObject();
}

BOOL CGGTongDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)
	
	
	return TRUE;
}

void CGGTongDoc::OnCloseDocument( )
{
	m_bAutoDelete = false;	// 关闭所有子窗口， 不销毁Document
	CDocument::OnCloseDocument();
}

void CGGTongDoc::SetTitle(LPCTSTR lpszTitle) 
{
	CString StrTitle = lpszTitle;
	POSITION pos = GetFirstViewPosition();
	while (pos != NULL)
	{
		CView* pView = GetNextView(pos);
		CWnd* pWnd = pView->GetParent();
		if ( NULL != pWnd )
		{
			pWnd->GetWindowText(StrTitle);
		}
	}
	CDocument::SetTitle(StrTitle);
}

/////////////////////////////////////////////////////////////////////////////
// CGGTongDoc serialization

void CGGTongDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}
/////////////////////////////////////////////////////////////////////////////
// CGGTongDoc diagnostics

#ifdef _DEBUG
void CGGTongDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CGGTongDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CGGTongDoc commands

bool32 CGGTongDoc::OnLoginDlgDoModalSuccess()
{
	return CreateAllGGTongObject();
}

bool32 CGGTongDoc::CreateAllGGTongObject(void)
{
	HWND hWndOwner = NULL;
	/*if (NULL != m_pDlgLogIn)
		hWndOwner = m_pDlgLogIn->GetSafeHwnd();
	
	ASSERT(NULL != hWndOwner);*/
	
	// 
//	DestroyAllGGTongObject();
	
	
	m_pAbsCenterManager = GetCenterManagerObj();
	if (NULL == m_pAbsCenterManager)
	{
		return false;
	}

	// 数据管理类
	m_pAbsDataManager = m_pAbsCenterManager->GetDataManager();
	if (!m_pAbsDataManager)
	{
		return false;
	}

	m_pAutherManager = m_pAbsCenterManager->GetProxyAuthManager();
	if (!m_pAutherManager)
	{
		return false;
	}

	m_pNewsManager = m_pAbsCenterManager->GetNewsManager();
	if (!m_pNewsManager)
	{
		return false;
	}
	return true;

}

void CGGTongDoc::DestroyAllGGTongObject(void)
{
	
	if (m_pAbsCenterManager)
	{
		m_pAbsCenterManager->Release();
	}

	//DEL(m_pAbsCenterManager);
	//DEL(m_pNewsManager);
    DEL(m_pHttpClient);
	
}


bool32 CGGTongDoc::Login()
{
	if (NULL != m_pDlgLogIn)
		return true;

#ifdef TRACE_DLG
	if ( NULL == m_pDlgbuding )
	{
		m_pDlgbuding = new CDlgbuding(NULL);
		m_pDlgbuding->Create(IDD_DIALOG_BUDING, NULL);
	}
#endif
	
	
	m_pDlgLogIn = new CDlgNewLogin(*this);
	int iLoginRet = m_pDlgLogIn->DoModal();
    m_isDefaultUser = m_pDlgLogIn->IsDefaultUser();

	DEL(m_pDlgLogIn);

	if (IDOK == iLoginRet)
		return true;
	
	return false;
}

void CGGTongDoc::PostInit()
{
	if (NULL != m_pAbsCenterManager)
	{
		m_ReportScheme.Construct(m_pAbsCenterManager->GetUserName());
		m_FaceScheme.Contruct(m_pAbsCenterManager->GetUserName());
	}
}

void CGGTongDoc::SetInfoCenterIP( const CString& StrAddress )
{
    if ( StrAddress.GetLength() <= 0 )
    {
        return;
    }

    m_StrInfoCenterIP = StrAddress;
	m_bReqMessageInfo = true;
}

CString CGGTongDoc::GetInfoCenterIP() 
{ 
	return m_StrInfoCenterIP; 
}

void CGGTongDoc::SetInfoCenterPort(int nPort)
{
	m_nInfoCenterPort = nPort;
}

int CGGTongDoc::GetInfoCenterPort()
{
	return m_nInfoCenterPort;
}


void CGGTongDoc::SetGateWayIP( const CString& StrAddress )
{
	if ( StrAddress.GetLength() <= 0 )
	{
		return;
	}

	m_StrGateWayIP = StrAddress;
}

CString CGGTongDoc::GetGateWayIP() 
{ 
	return m_StrGateWayIP; 
}

void CGGTongDoc::SetGateWayPort(int nPort)
{
	m_nGateWayPort = nPort;
}

int CGGTongDoc::GetGateWayPort()
{
	return m_nGateWayPort;
}



void CGGTongDoc::SetPickModelIP( const CString& StrAddress )
{
	if ( StrAddress.GetLength() <= 0 )
	{
		return;
	}

	m_StrPickModelIP = StrAddress;
}

CString CGGTongDoc::GetPickModelIP() 
{ 
	return m_StrPickModelIP; 
}

void CGGTongDoc::SetPickModelPort(int nPort)
{
	m_nPickModelPort = nPort;
}

int CGGTongDoc::GetPickModelPort()
{
	return m_nPickModelPort;
}


void CGGTongDoc::SetQueryStockIP( const CString& StrAddress )
{
	if ( StrAddress.GetLength() <= 0 )
	{
		return;
	}

	m_StrQueryStockIP = StrAddress;
}

CString CGGTongDoc::GetQueryStockIP() 
{ 
	return m_StrQueryStockIP; 
}

void CGGTongDoc::SetQueryStockPort(int nPort)
{
	m_nQueryStockPort = nPort;
}

int CGGTongDoc::GetQueryStockPort()
{
	return m_nQueryStockPort;
}



void CGGTongDoc::SetEconoServerURL(std::string strURL)
{
	m_strEconoServerURL = strURL;
	m_bReqEcono = true;
}

void CGGTongDoc::GetEconoServerURL(std::string &strURL)
{
	strURL = m_strEconoServerURL;
}

bool32 CGGTongDoc::IsReqEcono()
{
	return m_bReqEcono;
}

bool32 CGGTongDoc::IsReqMessageInfo()
{
	return m_bReqMessageInfo;
}

