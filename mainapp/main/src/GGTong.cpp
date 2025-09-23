// GGTong.cpp : Defines the class behaviors for the application.
//
#include "stdafx.h"
//#ifdef _DEBUG
//#include "vld.h"
//#endif // _DEBUG

#include "ShareFun.h"
#include "GGTongView.h"
#include "MPIDocTemplate.h"
#include "MPIChildFrame.h"
#include "AdjustViewLayoutDlg.h"
#include "PathFactory.h"
#include "IoViewSyncExcel.h"
#include "io.h"
#include "ReportScheme.h"
#include "dlgresponsibility.h"
#include "ConfigInfo.h"
#include "FontInstall.h"
#include "RichEditFormular.h"
#include "DIAEDITZBGSHS.h"
#include "coding.h"
#include "DumpFile.h"
#include "facescheme.h"
#include "WspManager.h"
#include "CCodeFile.h"
#include "WebClient.h"
#include "BridgeWnd.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// �������
HWND g_hwndTrace = NULL;

/////////////////////////////////////////////////////////////////////////////
// (1)������һ������
// (2)�ж�����һ�������Ƿ�����(����)
// (3)�ж��ϴ��Ƿ������ر�

const TCHAR* KGGTongInstanceGUID	= _T("0x28337fe0, 0xd2c6, 0x4b78, 0xa9, 0xf, 0xf9, 0xd4, 0x65, 0x37, 0xf, 0xbb");
const TCHAR* KGGTongFlagFile		= _T("ggtong.flag");

const BOOL KForceExist			= TRUE;

class CProcessSingleton
{
public:
	CProcessSingleton()
	{
		m_hMutex		= NULL;
		m_strMutex[0]	= '\0';
		m_bLastFlag		= FALSE;
		m_bGetLastFlag  = FALSE;

		// �����粻ָ�����ƣ���ʹ��exeĿ¼��crc32��Ϊ���� xl  1014
		const DWORD dwCount = sizeof(m_strMutex)/sizeof(m_strMutex[0]);
		DWORD dwRet = GetModuleFileName(NULL, m_strMutex, dwCount);
		if ( dwCount == dwRet )
		{
			m_strMutex[dwCount-1] = _T('\0');
			dwRet--;
		}
		ASSERT( dwRet > 0 && dwRet <= MAX_PATH );
		while ( dwRet > 0 )
		{
			if ( m_strMutex[dwRet-1] == _T('\\') ||  m_strMutex[dwRet-1] == _T('/') )
			{
				m_strMutex[dwRet-1] = _T('\0');	// ��ȡĿ¼
				break;
			}
			--dwRet;
		}
		TRACE(_T("\r\nEvent Path: %s"), m_strMutex);

		DWORD dwCrc32 = CCodeFile::crc_32((const char *)m_strMutex, dwRet*sizeof(m_strMutex[0]));
		_sntprintf(m_strMutex, dwCount, _T("c1f0jxd2,%08X"), dwCrc32);
		TRACE(_T("  Name: %s\r\n"), m_strMutex);
	}
public:
	CProcessSingleton( const TCHAR* strInstance,BOOL bGetLastFlag = TRUE)
	{
		_tcscpy(m_strMutex,strInstance);
		m_hMutex		= NULL;
		m_bLastFlag		= FALSE;
		m_bGetLastFlag	= bGetLastFlag;
	}
	~CProcessSingleton()
	{
		if ( NULL != m_hMutex )
		{
			DEL_HANDLE( m_hMutex );
			m_hMutex = NULL;
		}
		if ( m_bGetLastFlag )
		{
			DeleteFlagFile();
		}
	}
	// ��������,�����������һ��ʵ��,���Զ��˳�.
	void TryStart()
	{
		m_hMutex = ::CreateMutex(NULL,FALSE,m_strMutex);		
		if( NULL == m_hMutex )
		{
			Exit();
		}
		if (ERROR_SUCCESS != GetLastError())
		{
			Exit();
		}
		if ( GetLastError()==ERROR_ALREADY_EXISTS)
		{
			Exit();
		}
		
		if ( m_bGetLastFlag )
		{
			if ( CheckFlagFile() )
			{
				m_bLastFlag = TRUE;
			}
			CreateFlagFile();
		}
	}
	// �ж��ϴ��Ƿ������ر�.������TryStart֮��ʹ��.
	BOOL GetLastFlag()
	{
		ASSERT(NULL!=m_hMutex);
		ASSERT(m_bGetLastFlag);
		return m_bLastFlag;
	}
	// �ж�һ�������Ƿ����.�κ�ʱ�����ʹ��.
	BOOL IsExist ( const TCHAR* strInstance )
	{
		BOOL bExist = FALSE;
		HANDLE hMutex = ::CreateMutex(NULL,FALSE,strInstance);
		if( NULL == hMutex )
		{
			bExist = TRUE;
		}
		if ( GetLastError()==ERROR_ALREADY_EXISTS)
		{
			bExist = TRUE;
		}
		if ( NULL != hMutex )
		{
			DEL_HANDLE(hMutex);
		}
		return bExist;
	}
private:
	void CreateFlagFile()
	{
		FILE* fp = _tfopen ( KGGTongFlagFile,_T("a+"));
		if ( NULL != fp )
		{
			fclose(fp);
		}
	}
	void DeleteFlagFile()
	{
		::DeleteFile(KGGTongFlagFile);
	}
	BOOL CheckFlagFile()
	{
		if ( 0 == _taccess(KGGTongFlagFile,0))
		{
			return TRUE;
		}
		return FALSE;
	}
	void Exit()
	{
		//lint --e{506}
		if ( KForceExist )
		{
			exit(0);
		}
		else
		{
			PostQuitMessage(0);
		}
	}
protected:
	HANDLE	m_hMutex;
	TCHAR	m_strMutex[1024];
	BOOL	m_bLastFlag;
	BOOL	m_bGetLastFlag;
};

//CProcessSingleton ProcessSingleton(KGGTongInstanceGUID);
CProcessSingleton ProcessSingleton;	// ʹ��exeĿ¼

CGGTongApp* CBindHotkey::m_pApp = NULL;
CBindHotkey::CBindHotkey()
{
	AddRef();
}
CBindHotkey::~CBindHotkey()
{
	Release();
}
void CBindHotkey::AddRef()
{
	CBindHotkey::m_pApp->m_iBindHotkey ++;
}
void CBindHotkey::Release()
{
	CBindHotkey::m_pApp->m_iBindHotkey --;
}

/////////////////////////////////////////////////////////////////////////////
// CGGTongApp

BEGIN_MESSAGE_MAP(CGGTongApp, CWinApp)
//{{AFX_MSG_MAP(CGGTongApp)
ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
ON_COMMAND(ID_RESPONSIBILITY, OnAppResponsibility)
// NOTE - the ClassWizard will add and remove mapping macros here.
//    DO NOT EDIT what you see in these blocks of generated code!
//}}AFX_MSG_MAP
// Standard file based document commands
ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
// Standard print setup command
ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGGTongApp construction

int g_iTickBegin = 0;
CGGTongApp::CGGTongApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
	g_iTickBegin = GetTickCount();
	CBindHotkey::m_pApp = this;
	m_iBindHotkey		= 0;	
	m_bBindHotkey		= true;
	m_iGroupID			= 0;
	m_bCustom			= FALSE;
	m_aWspFileInfo.RemoveAll();

	m_strNewsInfo1		= L"ʵʱ����";
	m_strNewsInfo2		= L"��Ʒ��Ѷ";
	m_strNewsInfo3		= L"�ƾ�����";

	m_bTradeExit		= FALSE;
    m_bUserLoginExit = FALSE;
    m_HQRegisterExit = FALSE;

//	HINSTANCE hAppIns = GetModuleHandle(NULL);
	m_cefApp = new ClientApp();
	m_cefApp->Init(m_hInstance);

}

/////////////////////////////////////////////////////////////////////////////
// The one and only CGGTongApp object

CGGTongApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CGGTongApp initialization
bool32 CGGTongApp::m_bOffLine = false;

bool32 CGGTongApp::LoadGdiPlus()
{
	GdiplusStartupInput gdiplusStartupInput;    
    //װ��gdi+
    if (Ok != GdiplusStartup(&m_pGdiToken,&gdiplusStartupInput,NULL))
    {
		// ʧ��!
		//ASSERT(0);
		return false;
    }
    
	return true;
}

void CGGTongApp::RealeseGdiPlus()
{
	GdiplusShutdown(m_pGdiToken);
}



void CGGTongApp::ScanAvaliableWspFile(const CString& StrUserName, OUT bool32& bNewUser)
{
	bNewUser = false;
	if ( StrUserName.GetLength() <= 0 )
	{
		return;
	}

	//
	m_aWspFileInfo.RemoveAll();

	CString StrDir = CPathFactory::GetPrivateWorkspacePath(StrUserName);

	if(StrDir.Right(1) != "/")		
	{
		StrDir += L"/";
	}

	StrDir += "*.wsp";
	
	CFileFind file;
	bool32 bContinue = file.FindFile(StrDir);

	while (bContinue)
	{
		bContinue = file.FindNextFile();
		
		CString StrfileDir   = file.GetFilePath();
		CString StrfileTitle = file.GetFileTitle();	
		
		// ��Ҫ��ʾ�ļ���,��ʾXML ������������

		string sPath = _Unicode2MultiChar(StrfileDir);
		const char* strFilePath = sPath.c_str();

		CString StrFileNameXML = CMainFrame::GetWspFileShowName(strFilePath);
		CString StrFileVersion = CMainFrame::GetWspFileVersion(strFilePath);
		bool32  bIsReadOnly    = CMainFrame::IsWspReadOnly(strFilePath);
		// 
		StrFileNameXML.TrimLeft();
		StrFileNameXML.TrimRight();
		
		if ( StrFileNameXML.GetLength() <= 0 )
		{
			StrFileNameXML = StrfileTitle;
		}	
		
		// ��������
		T_WspFileInfo WspFileInfo;
		WspFileInfo.m_StrFileName		= StrfileTitle;
		WspFileInfo.m_StrFilePath		= StrfileDir;
		WspFileInfo.m_StrFileVersion	= StrFileVersion;
		WspFileInfo.m_StrFileXmlName	= StrFileNameXML;
		WspFileInfo.m_bReadOnly			= bIsReadOnly;
		//
		m_aWspFileInfo.Add(WspFileInfo);
	}
	
	//////////////////////////////////////////////////////////////////////////
	// �������������û�,��ʾ����WORKSPACE ������
	
	if ( m_aWspFileInfo.GetSize() == 0 )
	{		
		bNewUser = true;
		StrDir = CPathFactory::GetPublicWorkspacePath();
		
		if(StrDir.Right(1) != "/")
			StrDir += L"/";
		StrDir += "*.wsp";
		
		CFileFind WspFile;
		bContinue = WspFile.FindFile(StrDir);
		while (bContinue)
		{
			bContinue = WspFile.FindNextFile();			
			CString StrfileDir   = WspFile.GetFilePath();
			CString StrfileTitle = WspFile.GetFileTitle();	
			
			// ��Ҫ��ʾ�ļ���,��ʾXML ������������

			string sPath = _Unicode2MultiChar(StrfileDir);
			const char* strFilePath = sPath.c_str();

			CString StrFileNameXML = CMainFrame::GetWspFileShowName(strFilePath);
			CString StrFileVersion = CMainFrame::GetWspFileVersion(strFilePath);	
			// 
			StrFileNameXML.TrimLeft();
			StrFileNameXML.TrimRight();
			
			if ( StrFileNameXML.GetLength() <= 0 )
			{
				StrFileNameXML = StrfileTitle;
			}
			
			// ��������
			T_WspFileInfo WspFileInfo;
			WspFileInfo.m_StrFileName		= StrfileTitle;
			
			// �����·��Ӧ�ñ����û�˽��·��,��Ϊ����������û���Ϣ,�������ǹ���Ŀ¼�Ļ�.�����д�������ǲ������Ŀ¼�Ĺ�����Ϣ,Bug
			CString StrPathPrivate = CPathFactory::GetPrivateWorkspacePath(StrUserName);
			StrPathPrivate	+= StrfileTitle;
			StrPathPrivate	+= ".wsp";

			WspFileInfo.m_StrFilePath		= StrPathPrivate;
			WspFileInfo.m_StrFileVersion	= StrFileVersion;
			WspFileInfo.m_StrFileXmlName	= StrFileNameXML;
			//
			m_aWspFileInfo.Add(WspFileInfo);
		}
	}
}

void CGGTongApp::GetWspFileNameArray(CStringArray& aFileNames)
{
	aFileNames.RemoveAll();

	for(int32 i = 0 ; i < m_aWspFileInfo.GetSize(); i++)
	{
		CString StrFileName = m_aWspFileInfo[i].m_StrFileName;
		aFileNames.Add(StrFileName);
	}
}

void CGGTongApp::GetWspFileXmlNameArray(CStringArray& aFileXmlNames)
{
	aFileXmlNames.RemoveAll();
	
	for(int32 i = 0 ; i < m_aWspFileInfo.GetSize(); i++)
	{
		CString StrFileXmlName = m_aWspFileInfo[i].m_StrFileXmlName;
		aFileXmlNames.Add(StrFileXmlName);
	}
}

void CGGTongApp::GetWspFilePathArray(CStringArray& aFilePaths)
{
	aFilePaths.RemoveAll();
	
	for(int32 i = 0 ; i < m_aWspFileInfo.GetSize(); i++)
	{
		CString StrFilePath = m_aWspFileInfo[i].m_StrFilePath;
		aFilePaths.Add(StrFilePath);
	}
}

CString CGGTongApp::GetWspFilePathByXmlName(const CString& StrXmlName)
{
	if ( StrXmlName.GetLength() <= 0 )
	{
		return L"";
	}

	for ( int32 i = 0 ; i < m_aWspFileInfo.GetSize(); i++)
	{
		if ( m_aWspFileInfo[i].m_StrFileXmlName == StrXmlName )
		{
			return m_aWspFileInfo[i].m_StrFilePath;
		}
	}

	return L"";
}

CString CGGTongApp::GetWspFilePathByFileName(const CString& StrFileName)
{
	if ( StrFileName.GetLength() <= 0 )
	{
		return L"";
	}
	
	for ( int32 i = 0 ; i < m_aWspFileInfo.GetSize(); i++)
	{
		if ( m_aWspFileInfo[i].m_StrFileName == StrFileName )
		{
			return m_aWspFileInfo[i].m_StrFilePath;
		}
	}
	
	return L"";
}

const T_WspFileInfo* CGGTongApp::GetWspFileInfo(const CString& KStrPath)
{
	if ( 0 == KStrPath.GetLength() )
	{
		return NULL;
	}

	CString StrFilePath(KStrPath);	// ������·��תΪȫ·��
	StrFilePath.TrimLeft();
	if ( StrFilePath.IsEmpty() )
	{
		return NULL;
	}
	if ( StrFilePath.GetLength() > 2 && (StrFilePath.Left(2) == _T(".\\") || StrFilePath.Left(2) == _T("./")) )
	{
		CString StrCurPath;
		::GetCurrentDirectory(MAX_PATH, StrCurPath.GetBuffer(MAX_PATH));
		StrCurPath.ReleaseBuffer();
		if ( !StrCurPath.IsEmpty() && StrCurPath.Right(1) == _T("\\") )
		{
			StrCurPath.Delete(StrCurPath.GetLength()-1);
		}
		StrFilePath.Delete(0);
		StrFilePath.Insert(0, StrCurPath);
	}

	// 
	for ( int32 i = 0; i < m_aWspFileInfo.GetSize(); i++ )
	{
		T_WspFileInfo InfoTest = m_aWspFileInfo[i];

		if ( InfoTest.m_StrFilePath.Find(_T('\\'), 0) != -1 )	// ���� \\ /ͳһ
		{
			StrFilePath.Replace(_T('/'), _T('\\'));
		}
		else
		{
			StrFilePath.Replace(_T('\\'), _T('/'));
		}
		
		if ( InfoTest.m_StrFilePath == StrFilePath )
		{
			return &m_aWspFileInfo[i];
		}
	}
	
	//
	return NULL;
}

void CGGTongApp::CopyPublicWCVFilesToPrivate(CString StrUserName)
{
	if ( StrUserName.GetLength() <= 0 )
	{
		CGGTongDoc* pDoc = (CGGTongDoc*)AfxGetDocument();		
		StrUserName = pDoc->m_pAbsCenterManager->GetUserName();
		
		if ( StrUserName.GetLength() <= 0 )
		{
			return;
		}
	}

	// �Ϸ��û���¼,������Ŀ¼�µ� wsp,cfm,vmg �ļ�������˽��Ŀ¼
	CString StrPublicWorkSpacePath = CPathFactory::GetPublicWorkspacePath();
	StrPublicWorkSpacePath += L"*.*";
	CFileFind filePublic;
	bool32 bContinue = filePublic.FindFile(StrPublicWorkSpacePath);
	
	while (bContinue)
	{
		bContinue = filePublic.FindNextFile();
		
		if (!filePublic.IsDirectory() && !filePublic.IsDots())
		{
			CString StrfileName  = filePublic.GetFileName();
			CString StrfilePath	 = filePublic.GetFilePath();

			// ������cfm�ļ�
			if ( StrfileName.GetLength() > 4 && StrfileName.Right(4).CompareNoCase(_T(".cfm")) == 0 )
			{
				continue;
			}
			
			// ������ļ���˽��Ŀ¼���Ƿ����,������ھͲ�����,���⸲���û��Զ�����޸�
			CString StrPrivateFile = CPathFactory::GetPrivateWorkspacePath(StrUserName) + StrfileName;
			
			TCHAR TStrPrivateFile[1024];
			lstrcpy(TStrPrivateFile,StrPrivateFile);
			_tcheck_if_mkdir(TStrPrivateFile);
			
			CFileFind filePrivate;
			bool32 bFind = filePrivate.FindFile(StrPrivateFile);
			if (!bFind)
			{
				// ���������,�򿽱�
				::CopyFile(StrfilePath,StrPrivateFile,true);
			}
		}
	}					
}

const T_WspFileInfo* CGGTongApp::GetWspFileInfo(const char* KStrPath)
{
	CString StrPath = (CString)KStrPath;

	if ( 0 == StrPath.GetLength() )
	{
		return NULL;
	}
	
	//
	return GetWspFileInfo(StrPath);
}

CGGTongApp::E_UserRightType CGGTongApp::GetUserRightType()
{
	E_UserRightType eUserRightType = EURTAll;
	
	bool32 bFuture = false;
	bool32 bStock  = false;

	// �õ��û���Ȩ������:
	CGGTongDoc* pDoc = (CGGTongDoc*)AfxGetDocument();
	if ( NULL == pDoc )
	{
		return eUserRightType;
	}

	//
	CAbsCenterManager* pAbsCenterManager = pDoc->m_pAbsCenterManager;
	if ( NULL == pAbsCenterManager )
	{
		return eUserRightType;
	}
	
	//
	for ( int32 i = 0; i < pAbsCenterManager->GetMerchManager().m_BreedListPtr.GetSize(); i++ )
	{
		CBreed* pBreed = pAbsCenterManager->GetMerchManager().m_BreedListPtr.GetAt(i);
		if ( NULL == pBreed )
		{
			continue;
		}

		for ( int32 j = 0; j < pBreed->m_MarketListPtr.GetSize(); j++ )
		{
			CMarket* pMarket = pBreed->m_MarketListPtr.GetAt(j);
			if ( NULL == pMarket )
			{
				continue;
			}

			if ( CReportScheme::IsFuture(pMarket->m_MarketInfo.m_eMarketReportType) )
			{
				bFuture = true;
			}
			else
			{
				bStock = true;
			}
		}
	}

	if ( bFuture && !bStock )
	{
		// �ڻ������û�
		eUserRightType = CGGTongApp::EURTFuture;
	}
	else if ( !bFuture && bStock )
	{
		// ��Ʊ�����û�
		eUserRightType = CGGTongApp::EURTStock;
	}
	else if ( bFuture && bStock )
	{
		// ������û�
		eUserRightType = CGGTongApp::EURTAll;
	}
	else 
	{
		// ����
		//ASSERT(0);
	}

	return eUserRightType;
}


CString CGGTongApp::GetFilesVersion(LPTSTR StrFileName /*= ""*/)    
{    
	if ( 0 == lstrlen(StrFileName) )
	{
		// ����ĳ����� 0, Ĭ��ȡ����Ӧ�ó���
		TCHAR aStrPath[MAX_PATH];	
		::GetModuleFileName(NULL, aStrPath, sizeof(aStrPath) / sizeof(TCHAR) ); //���Ȼ�ð汾��Ϣ��Դ�ĳ���		
		StrFileName = aStrPath;
	}
	
	//
	int		iVerInfoSize;   
    char	*pBuf;   
    CString StrVersion = L"";   
	
    VS_FIXEDFILEINFO	*pVsInfo;   
    UINT	iFileInfoSize = sizeof(VS_FIXEDFILEINFO);   
	
    iVerInfoSize   =   GetFileVersionInfoSize(StrFileName, NULL);    
	
    if( iVerInfoSize != 0 )    
    {      
        pBuf = new char[iVerInfoSize];   
		
		if( GetFileVersionInfo(StrFileName, 0, iVerInfoSize, pBuf) )      
        {      
            if( VerQueryValue(pBuf, L"\\", (void**)&pVsInfo, &iFileInfoSize) )      
            {      
                StrVersion.Format(L"%d.%d.%d.%d", HIWORD(pVsInfo->dwFileVersionMS), LOWORD(pVsInfo->dwFileVersionMS), HIWORD(pVsInfo->dwFileVersionLS), LOWORD(pVsInfo->dwFileVersionLS));   
            }      
        }      
        
		delete []pBuf;      
    }      
	
    return   StrVersion;  	
} 

CString CGGTongApp::GetFilesLastModifyTime(LPTSTR StrFileName)
{
	CString StrLastModifyTime = L"";
	
	if ( 0 == lstrlen(StrFileName) )
	{
		// ����ĳ����� 0, Ĭ��ȡ����Ӧ�ó���
		TCHAR aStrPath[MAX_PATH];	
		::GetModuleFileName(NULL, aStrPath, sizeof(aStrPath) / sizeof(TCHAR) ); //���Ȼ�ð汾��Ϣ��Դ�ĳ���		
		StrFileName = aStrPath;
	}
	
	//
	WIN32_FIND_DATA ffd ;   
    FindFirstFile(StrFileName, &ffd);  
	
    SYSTEMTIME stUTC, stLocal;   
    FileTimeToSystemTime(&(ffd.ftLastWriteTime), &stUTC);   
    SystemTimeToTzSpecificLocalTime(NULL, &stUTC, &stLocal);   
	
    StrLastModifyTime.Format(L"%d/%d/%d", stLocal.wYear, stLocal.wMonth, stLocal.wDay);   
	
    //
    return StrLastModifyTime; 
}

BOOL CGGTongApp::InitInstance()
{
	int iTickBegin = GetTickCount();
	
	SetCurrentMoudlePath();
	setlocale( LC_ALL, "chs" ); 

	::CoInitialize(NULL);
	
	ProcessSingleton.TryStart();

	{
	
		// ������Ŀ¼�����ٳ���ʱ������dump�ļ��ļ���
		CString StrDumpPath(_T("dump\\"));
		_tcheck_if_mkdir(StrDumpPath.GetBuffer(MAX_PATH));
		StrDumpPath.ReleaseBuffer();
		SetUnhandledExceptionFilter(MyUnhandledFilter);
	
	}

	
	//
	if (!AfxOleInit())
	{
		return FALSE;
	}

	// ÿ�ε�½����ѻ�����
	{
		
		_tremove_dir(_T("vipdata"));
		
	}

	//
	if ( !LoadGdiPlus())
	{
		return FALSE;
	}	

	
	// ��װ��Ҫ������
	CFontInstallHelper::InstallFonts();		
	
	//
	AfxEnableControlContainer();
	AfxInitRichEdit();

	// 
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.
	Enable3dControls();			// Call this when using MFC in a shared DLL
	
#ifndef _AFXDLL
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	///> ��ʼ��һЩȫ�ֶ���
	OnGlobalInit();
	// Change the registry key under which our settings are stored.
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization.
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));
	
	LoadStdProfileSettings();  // Load standard INI file options (including MRU)
	
	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.
	// ���ݰ汾����app���� - ȡ��
// 	s_StrAppName = CVersionInfo::Instance()->GetVersionName();
// 	AfxGetApp()->m_pszAppName = s_StrAppName;
	
	{	
		CString StrImagePath = CPathFactory::GetImagePath();
		_tcheck_if_mkdir(StrImagePath.GetBuffer(MAX_PATH));		// ����ͼƬĿ¼
		StrImagePath.ReleaseBuffer();	
	}
	
	// ���������ļ�, ���ó�����ʾ��Ĭ������
	m_pConfigInfo = CConfigInfo::Instance();
	if ( m_pConfigInfo->LoadSysConfig() )
	{
		//free((void*)m_pszAppName); // ��֪����Щ�ط��������ã�ֻ��й©��
		AfxGetApp()->m_pszAppName = _tcsdup(m_pConfigInfo->m_StrAppNameConfig);
		AfxGetModuleState()->m_lpszCurrentAppName = AfxGetApp()->m_pszAppName;
	}	
	
	// create the MPI document template
	m_pDocTemplate = new CMPIDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CGGTongDoc),
		RUNTIME_CLASS(CMPIChildFrame));
	AddDocTemplate(m_pDocTemplate);
	
	CNewMenu::SetMenuDrawMode(CNewMenu::STYLE_XP);
	CNewMenu::SetMenuBkColor(RGB(25, 24, 30));
	CNewMenu::SetMenuBitmapBkColor(RGB(25, 24, 30));
	CNewMenu::SetMenuItemSelColor(RGB(35, 34, 40));
	CNewMenu::SetMenuBorderColor(RGB(5, 4, 10));
	CNewMenu::SetMenuItemNorTextColor(RGB(190, 189, 193));
	CNewMenu::SetMenuItemSelTextColor(RGB(209, 61, 65));
	CNewMenu::SetMenuItemDisableTextColor(RGB(168,168,168));
	
	//CNewMenu::SetMenuDrawMode(GetProfileInt(_T("NewMenu"),_T("Style"),3));
	CNewMenu::SetSelectDisableMode(GetProfileInt(_T("NewMenu"),_T("Selection"),1));
 	//CNewMenu::SetXpBlendig(GetProfileInt(_T("NewMenu"),_T("Gloom"),1));

	// ȥ���˵���
	m_pDocTemplate->m_hMenuShared=NULL;

	//AfxGetApp()->m_pszAppName = s_StrAppName;	// �ٴ�����~
	
	// create the document
	
	m_pDocument = (CGGTongDoc *)m_pDocTemplate->CreateNewDocument();
	if (NULL == m_pDocument)
		return FALSE;		
	// ������¼����
	OfflineLogin();

	//	��������������
    m_pConfigInfo->LoadToolBarConfig();	
	
		// �������õ�ָ�����ļ�
	m_pConfigInfo->LoadIndexGroupConfig();			

	//	���������û����壬�б�ͷ
	m_pDocument->PostInit();
	CReportScheme::Instance()->SetPrivateFolderName(m_pDocument->m_pAbsCenterManager->GetUserName());

	// ��ʼ�������û�����ҳ����Ϣ	
	CCfmManager::Instance().Initialize(m_pDocument->m_pAbsCenterManager->GetUserName());
	
	
	// create main MDI Frame window
	CMainFrame* pMainFrame = new CMainFrame;
	if (!pMainFrame->LoadFrame(IDR_MAINFRAME))
	{
		return FALSE;
	}
	


	m_pMainWnd = pMainFrame;	

	m_pDocument->m_pAbsCenterManager->SetFrameHwnd(pMainFrame->GetSafeHwnd());
	COptimizeServer::Instance()->SetOwner(pMainFrame->GetSafeHwnd());

  
	// ֪ͨMainframe, ���²˵�

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	
	// Dispatch commands specified on the command line
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;


	// �����հ״���
	{
	
		pMainFrame->OnWindowNew();
		
	}	

	// ���ó�ʼ����������
	pMainFrame->ShowWindow(SW_SHOWMAXIMIZED);	// ���
	pMainFrame->UpdateWindow();	
	pMainFrame->SetFocus();	
	// ����Ĭ�Ϲ���ҳ��
	{
		CString StrDefaultCfmName = CCfmManager::Instance().GetUserDefaultCfm().m_StrXmlName;
		if ( !StrDefaultCfmName.IsEmpty() )
		{
			CCfmManager::Instance().LoadCfm(StrDefaultCfmName);
		}
	}
	pMainFrame->OnCloseAllWindow();


	return TRUE;
}

BOOL CGGTongApp::PreTranslateMessage(MSG* pMsg) 
{	
	if ( m_bBindHotkey && m_iBindHotkey > 0 )
	{ 
		CMainFrame* pMainFrm = (CMainFrame*)AfxGetMainWnd();
		if ( NULL != pMainFrm )
		{
			pMainFrm->ProcessHotkey(pMsg);
		}
	}

	return CWinApp::PreTranslateMessage(pMsg);
}

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();
	~CAboutDlg();

private:
	Image		*m_pImage;
	CRect		m_rcTop;
	CRect		m_rcBottom;
	CWndCef		m_wndCef;
//---Begin-- �ظ����壬PC-Lint����
//	HBRUSH		m_BrushBack;
//---End--
	bool32		m_bShowWeb;					// ����ַ��ʱ��, ����ͼƬ


public:
	// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };	
	CString m_StrConfig1;
	CString m_StrConfig2;
	CString m_StrData;
	// CString m_StrLMTime;
	//}}AFX_DATA
	
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL
	
	// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	// No message handlers
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD, NULL)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
	m_pImage = NULL;
	m_BrushBack = CreateSolidBrush(0x4d4137);
	m_bShowWeb = false;
}

CAboutDlg::~CAboutDlg()
{
	DEL(m_pImage);
	DeleteObject(m_BrushBack);
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	DDX_Text(pDX, IDC_STATIC_1, m_StrConfig1);
	DDX_Text(pDX, IDC_STATIC_2, m_StrConfig2);	
	DDX_Text(pDX, IDC_STATIC_DATA, m_StrData);
	// DDX_Text(pDX, IDC_STATIC_LMTIME, m_StrLMTime);
	//}}AFX_DATA_MAP
}

BOOL CAboutDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	int32 iWndWidth = 625;
	int32 iWndHeight = 470;
	int32 iTopHeight = 80;
	int32 iBottomHeight = 360;

	CString StrName = L"����";
	CGGTongApp* pApp = (CGGTongApp*)AfxGetApp();
	if (NULL != pApp)
	{
		int32 iSize = pApp->m_pConfigInfo->m_aWndSize.GetSize();
		for (int32 i=0; i<iSize; i++)
		{
			CWndSize wnd = pApp->m_pConfigInfo->m_aWndSize[i];
			if (L"����" == wnd.m_strID)
			{
				StrName = wnd.m_strTitle;
				iWndWidth = wnd.m_iWidth;
				iWndHeight = wnd.m_iHeight;
				iBottomHeight = iWndHeight - iTopHeight - 30;
				break;
			}
		}
	}

	CRect rcWnd(0, 0, iWndWidth, iWndHeight);
	
	m_rcTop = rcWnd;
	m_rcTop.bottom = iTopHeight;
	
	m_rcBottom = rcWnd;
	m_rcBottom.top = m_rcTop.bottom;
	m_rcBottom.bottom = m_rcBottom.top + iBottomHeight;
	
	MoveWindow(rcWnd);
	CenterWindow();

	if ( NULL != pApp )
	{
		// SetWindowText(_T("���� ") + pApp->m_pConfigInfo->m_StrAppNameConfig);
		SetWindowText(StrName);

		m_StrConfig1	= pApp->m_pConfigInfo->m_StrAboutStr1;
		m_StrConfig2	= pApp->m_pConfigInfo->m_StrAboutStr2;
	}
	
	// �û�������
	if (NULL != pApp && NULL != pApp->m_pDocument->m_pAbsCenterManager )
	{
		iServiceDispose* p = pApp->m_pDocument->m_pAbsCenterManager->GetServiceDispose();
		if ( NULL != p )
		{
			T_UserInfo stUserInfo;			
			p->GetUserInfo(stUserInfo);

		//	m_StrData = L"�û�������      " + (CString)stUserInfo.szExpirationDate;
		}
	}

	//
	if ( CGGTongApp::m_bOffLine )
	{
		GetDlgItem(IDC_STATIC_DATA)->ShowWindow(SW_HIDE);
	}

	m_pImage = Image::FromFile(CPathFactory::GetImageAboutImgPath());
	
	//--- wangyongxue Ŀǰʹ�ñ���ͼƬ������ʾ
// 	CString strAdvsCode = pApp->m_pConfigInfo->m_StrAboutUsAdvsCode;
// 	CString StrAdvsUrl  = pApp->m_pConfigInfo->GetAdvsCenterUrlByCode(strAdvsCode,EAUTPit);

// 	if (StrAdvsUrl.GetLength() > 0)
// 	{
// 		m_wndCef.SetUrl(StrAdvsUrl);
// 		m_wndCef.Create(NULL,NULL, WS_CHILD|WS_VISIBLE, m_rcBottom, this, 1988);
// 
// 		m_bShowWeb = true;
// 	}
// 	else
// 	{
// 		m_bShowWeb = false;
// 	}

	UpdateData(FALSE);
	//
	return TRUE;
}

HBRUSH CAboutDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	pDC->SetTextColor(0xdbd9d7);
	if (nCtlColor == CTLCOLOR_STATIC || nCtlColor == CTLCOLOR_BTN)
	{
		pDC->SetBkMode(TRANSPARENT);
		return m_BrushBack;
	}

	return hbr;
}

void CAboutDlg::OnPaint() 
{
	CPaintDC dc(this);
	dc.FillSolidRect(m_rcTop, 0x4d4137);

	//
	CDC memDC;
	memDC.CreateCompatibleDC(&dc);
	CBitmap bmp;
	CRect rcWindow;
	GetWindowRect(&rcWindow);
	rcWindow.OffsetRect(-rcWindow.left, -rcWindow.top);
	
	bmp.CreateCompatibleBitmap(&dc, rcWindow.Width(), rcWindow.Height());
	memDC.SelectObject(&bmp);
	memDC.SetBkMode(TRANSPARENT);
	Gdiplus::Graphics graphics(memDC.GetSafeHdc());
	
	RectF fRect;
	int32 iWidth = m_rcBottom.Width();
	int32 iHeight = m_rcBottom.Height();
	fRect.X = (REAL)0;
	fRect.Y = (REAL)0;
	fRect.Width = (REAL)iWidth;
	fRect.Height = (REAL)iHeight;
	if (NULL != m_pImage && !m_bShowWeb)
	{
		graphics.DrawImage(m_pImage, fRect, 0, 0, (REAL)m_pImage->GetWidth(), (REAL)m_pImage->GetHeight(), UnitPixel);
	}
	
	dc.BitBlt(m_rcBottom.left, m_rcBottom.top, m_rcBottom.Width(), m_rcBottom.Height(), &memDC, 0, 0, SRCCOPY);
	dc.SelectClipRgn(NULL);
	memDC.DeleteDC();
	bmp.DeleteObject();
}

void CAboutDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	if(m_rcBottom.PtInRect(point))
	{
		CGGTongApp* pApp = (CGGTongApp*)AfxGetApp();
		CString StrAdvsCode = pApp->m_pConfigInfo->m_StrAboutUsAdvsCode;
		CString StrAdvsUrl = pApp->m_pConfigInfo->GetAdvsCenterUrlByCode(StrAdvsCode,EAUTLink);
		if(!StrAdvsUrl.IsEmpty())
			ShellExecute(0, L"open", StrAdvsUrl, NULL, NULL, SW_NORMAL);
	}

	CDialogEx::OnLButtonDown(nFlags, point);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
//{{AFX_MSG_MAP(CAboutDlg)
// No message handlers
	ON_WM_CTLCOLOR()
	ON_WM_PAINT()
//}}AFX_MSG_MAP
ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

// App command to run the dialog
void CGGTongApp::OnAppAbout()
{
	//--- wangyongxue Ŀǰʹ�ñ���ͼƬ������ʾ
	CString strAdvsCode = m_pConfigInfo->m_StrAboutUsAdvsCode;
	CString StrAdvsUrl  = m_pConfigInfo->GetAdvsCenterUrlByCode(strAdvsCode,EAUTPit);
	if (!StrAdvsUrl.IsEmpty())
	{
		WebClient webClient;
		webClient.SetTimeouts(100, 200);
		webClient.DownloadFile(StrAdvsUrl,  L"image\\About.png");
	}
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}
void CGGTongApp::OnAppResponsibility()
{
	CDlgResponsibility Dlg;
	Dlg.DoModal();
}

void CGGTongApp::ReStart()
{
	STARTUPINFO si; 

	//���ؽ�����Ϣ
	PROCESS_INFORMATION pi; 		 	
	si.cb = sizeof(STARTUPINFO); 
	GetStartupInfo(&si); 
	si.wShowWindow = SW_SHOW; //���������д���
	si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;

	tchar szPath[512];
	GetModuleFileName(AfxGetApp()->m_hInstance, szPath, sizeof(szPath));

	//������ȡ�����н���
	CreateProcess(NULL, szPath, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi ); 
}

int CGGTongApp::ExitInstance()
{
	m_pConfigInfo->SaveSysConfig();	
	CConfigInfo::DelInstance();		// ɾ���汾��Ϣ

    m_cefApp->Exit();	// �ر�CEF

	// ��¼���󣬾Ͳ������е����ˣ�Ӧ����һЩ����
//#ifdef _DEBUG
	if ( NULL != m_pDocManager )
	{
		m_pDocManager->CloseAllDocuments(TRUE);		// ���doc��ϵ���ͷ�doc��Դ
		delete m_pDocument;
		m_pDocument = NULL;
		delete m_pDocManager;
		m_pDocManager = NULL;
	}
//#endif

	::CoUninitialize();

	// ģ�⽻���е������˺ŵ�¼���������������������
	if (m_bTradeExit || m_bUserLoginExit ||m_HQRegisterExit)
	{
		ReStart();
	}

	return CWinApp::ExitInstance();
}


/*
	 error 1540: (Warning -- Pointer member 'CGGTongApp::m_pDocument'
	 neither freed nor zeroed by destructor -- Effective C++ #6)
*/
//lint --e{1540}
CGGTongApp::~CGGTongApp()
{
}

BOOL CGGTongApp::OnIdle( LONG lCount )
{
	BOOL b = CWinApp::OnIdle(lCount);
	if ( lCount <= 0 )
	{
		// �����Popup&show���ڣ�����һ��WM_IDLEUPDATECMDUI
		HWND hWnd = AfxGetMainWnd()->GetSafeHwnd();	// ���������Ϊ��׼
		if ( NULL != hWnd )
		{
			//int32 iCount = 0; 
			//int32 iMsg = 0;
			while ( hWnd != NULL )
			{
				CWnd *pWnd = CWnd::FromHandlePermanent(hWnd);
				if ( NULL != pWnd && pWnd != m_pMainWnd && !pWnd->IsFrameWnd() )
				{
					// ��frame���ڣ�frame�����е����Ķ���
					DWORD dwExStyle = pWnd->GetStyle();
					if ( (dwExStyle & WS_POPUP) != 0 && IsWindowVisible(hWnd) )
					{
						pWnd->SendMessage(WM_IDLEUPDATECMDUI, (WPARAM)TRUE, 0);
						//iMsg++;
					}
				}
				hWnd = GetWindow(hWnd, GW_HWNDPREV);	// ���������
				//iCount++;
			}
			//TRACE(_T("����%d/%d\r\n"), iMsg, iCount);
		}
	}


	return b;
}


CGGTongDoc* AfxGetDocument()
{
	CGGTongApp* pApp = (CGGTongApp*)AfxGetApp();
	return pApp->m_pDocument;
}
/////////////////////////////////////////////////////////////////////////////
// CGGTongApp message handlers


//////////////////////////////////////////////////////////////////////////
// ���⹤���� - ��˽�б��棬���ǲ�������̳����⹤�����İ汾��
//	�ܲ�����Ϊ���򿪵Ĺ������أ� - ���������Ϊ���򿪣�����Ĭ�ϴ򿪿�����ǰ�߱�Ȩ�ޣ�������Ȩ�޵Ĺ�����
bool32 T_WspFileInfo::IsSpecialWsp() const
{
	return m_StrFileVersion.Find(_T("100.")) == 0;		// ��100.��ʼ����Ϊ���⹤����
}

CString T_WspFileInfo::GetNormalWspFileVersion() const
{
	if ( IsSpecialWsp() )
	{
		return _T("1.") + m_StrFileVersion.Mid(_tcslen(_T("100.")));	// ��1�滻���汾��
	}
	else
	{
		return m_StrFileVersion;
	}
}

T_WspFileInfo::T_WspFileInfo()
{
	m_bReadOnly = false;
}

void CGGTongApp::OnGlobalInit()
{
	CFaceScheme::SetIoViewObjects(CIoViewManager::GetIoViewObjectCount(),CIoViewManager::GetIoViewObject());
	CRichEditFormular::SetDataFunc(CDialogEDITZBGSHS::GetDataFunc(),CDialogEDITZBGSHS::GetDataFuncCount());
}

void CGGTongApp::SetCurrentMoudlePath()
{
	TCHAR TStrModule[_MAX_PATH];
	TCHAR TStrCurrentFilePath[MAX_PATH];

	GetModuleFileName(NULL, TStrModule, _MAX_PATH);
	CString StrTempPath = TStrModule;
	CString strModulePath = StrTempPath.Left(StrTempPath.ReverseFind('\\'));
	// ��ȡ��ǰ·��
	::GetCurrentDirectory(MAX_PATH-1, TStrCurrentFilePath);
	
	if (0 != strModulePath.CompareNoCase(TStrCurrentFilePath))
	{
		// �������õ�ǰ·��
		::SetCurrentDirectory(strModulePath.GetString());
	}
}