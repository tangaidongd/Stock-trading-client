#include "stdafx.h"

#include "ShareFun.h"
#include "MainFrm.h"
#include "dlgsysupdate.h"
#include "UserMsgDef.h"
#include <afxinet.h>

#include "AutoUpdate.h"
#include "pathfactory.h" 

//----------------------------------------------------------------------
//const CString KStrUpdateIniFile			 =  L"./update.ini"; 
//const CString KStrHttpListFile			 =  L"./config/http_list.ini";

//const CString KStrUpdateIniFile			 =  CPathFactory::GetUpdateFilePath();
//const CString KStrHttpListFile			 =  CPathFactory::GetHttpListFilePath();

const CString KStrDefaultHttpServer			 =  L"http://218.80.251.118:80/ggtong_client";
// 常量定义,所有可能需要升级的模块名称
/*#define  iModualNameSize 14
const TCHAR* aStrModualName [iModualNameSize]=
{
	// 静默升级， 升级完， 不需要做任何处理
	L"auth.xml",
	L"http_list.ini",
	L"info_address.ini",
	
	// 静默升级， 升级完做相应处理
	L"index.xml",
	L"sys_blocks.xml",
	L"relative_merchs.xml",
	L"hk_warrants.xml",

	// 提示升级， 升级完重启软件
	L"ggtong.exe",
	L"loader.exe",
	L"commengine.dll",
	L"dataengine.dll",	
	L"sockengine.dll",
	L"fileengine.dll",
	L"proxy_auth_client.dll",	
};
*/
//////////////////////////////////////////////////////////////////////////
extern bool32 gbStopDownloadThreadProc;
extern HANDLE sDownloadSilenceThreadHandle;
extern HANDLE sTestSpeedThreadHandle;

static const T_UpdateFilesInfo s_astUpdateFilesInfo[] = 
{
	T_UpdateFilesInfo(L"http_list.ini",			L"./config/http_list.ini"),	
	T_UpdateFilesInfo(L"info_address.ini",		L"./config/info_address.ini"),
	T_UpdateFilesInfo(L"sys_index.xml",			L"./config/sys_index.xml"),
	T_UpdateFilesInfo(L"flag_index.xml",		L"./config/flag_index.xml"),
	T_UpdateFilesInfo(L"sys_blocks.xml",		L"./config/sys_blocks.xml"),
	T_UpdateFilesInfo(L"relative_merchs.xml",	L"./config/relative_merchs.xml"),
	T_UpdateFilesInfo(L"hk_warrants.xml",		L"./config/hk_warrants.xml"),
	// 服务器上这个目录下放置UpdateLog.txt,程序员对这次系统更新做的提示性日志
};

const int32 KiUpdateFilesCounts = sizeof(s_astUpdateFilesInfo)/sizeof(T_UpdateFilesInfo);

//////////////////////////////////////////////////////////////////////////
CAutoUpdate::CAutoUpdate(CWnd &OwnerWnd)
:m_OwnerWnd(OwnerWnd)
{
	m_bAutoUpdate			= true;
	//m_bMoveFilesCompelete	= false;
	//m_bForceUpdate		= false;
	m_pDlgSysUpdate			= NULL;

	m_KStrUpdateIniFile			 =  CPathFactory::GetUpdateFilePath();
	m_KStrUpdateLog				 =  CPathFactory::GetUpdateLogPath();
	m_KStrHttpListFile			 =  CPathFactory::GetHttpListFilePath();
}

CAutoUpdate::~CAutoUpdate()
{

}

void CAutoUpdate::TryToUpdate()
{
	_LogWithTime("\n --------------------------------------------------------------华丽的分割线--------------------------------------------------------------\n\n");
	_LogWithTime(L"BeginUpdate!!!!!!!!!!!!!!!!!!!\n");
	
	//////////////////////////////////////////////////////////////////////////
	// 从 http_list.ini 读取服务器列表.判断哪个最快,再从该服务器下载update.ini
	// 和其他用于更新的文件;
	
	m_aHostAddressList.RemoveAll();
	for (int32 i = 0;i < HTTPSERVERNUMS ; i++)
	{
		CString StrSectionName,StrHttpServerAddress;
		CString StrLog;

		TCHAR   TStrHttpServerAddress[1024];
		StrSectionName.Format(L"%s%d",L"httpserver",i);
		
		::GetPrivateProfileString(StrSectionName,L"URL",L"",TStrHttpServerAddress,1024,m_KStrHttpListFile);
		StrHttpServerAddress.Format(L"%s",TStrHttpServerAddress);
		if (StrHttpServerAddress.GetLength() > 0)
		{
			StrLog.Format(L"ReadDate from httplist.ini. URL%d : %s\n",i,StrHttpServerAddress);
			_LogWithTime(StrLog);
			m_aHostAddressList.Add(StrHttpServerAddress);
		}		
	}
	
	// 读完http_list.ini 文件,测试这些HttpServer 的速度;
	TestServerSpeed(m_aHostAddressList); 		
}

void CAutoUpdate::TestServerSpeed(IN CStringArray& aHostServerList)
{	
	// 测速函数
	CString StrFastestHttpServer;

	_LogWithTime(L"CAutoUpdate::TestServerSpeed() ,if read http_list.ini false,use default url,else goto thread testspeed func.\n");

	int32 iSize = aHostServerList.GetSize();
	if (0 == iSize)
	{
		// 读http_list.ini文件失败,使用默认的地址,从这个地址下载update.ini

		_LogWithTime(L"read http_list.ini file false,use default url http://218.80.251.118:80/ggtong_client  download update.ini\n");

		m_StrFastestServer  = KStrDefaultHttpServer ;
		CString	StrTestFile = KStrDefaultHttpServer + L"/update.ini";
		CString StrLocal	= m_KStrUpdateIniFile;
		
		TCHAR TStrTestFile[1024];
		TCHAR TStrLocal[1024];
		
		lstrcpy(TStrTestFile,StrTestFile);
		lstrcpy(TStrLocal,StrLocal);

 		int32 iResult = HttpDownLoad(TStrTestFile,TStrLocal,NULL);
		m_OwnerWnd.PostMessage(UM_AutoUpdate_TestSpeedSuccess,0,0);
	}
	else
	{
		_LogWithTime(L"read http_list.ini file success,test speed\n");
		// 启动线程函数,通过下载update.ini测试,得到最快的服务器地址
		_LogWithTime("download update.ini to find the fastest http server.");
		CWinThread* pThread =AfxBeginThread(CAutoUpdate::TestServerSpeedThreadProc,this);
		sTestSpeedThreadHandle = pThread->m_hThread;
	}
}

UINT CAutoUpdate::TestServerSpeedThreadProc(LPVOID pParam )
{
	// 测速线程函数
	CAutoUpdate* pAutoUpdate = (CAutoUpdate*)pParam;
	ASSERT(NULL != pAutoUpdate);

	pAutoUpdate->TestServerSpeedThreadProc();
	
	return 0;
}
 
void CAutoUpdate::TestServerSpeedThreadProc()
{
	CString StrLog;
	int32 i, iSize = m_aHostAddressList.GetSize();
	StrLog.Format(_T("host address size = %d"),iSize);
	_LogWithTime(StrLog);
	bool32 bFoundFastAddress = false;
	for ( i = 0; i < iSize; i ++ )
	{
		//依次测试...
		CString StrTestFile = m_aHostAddressList[i] + L"/update.ini";
		CString StrLocal	= m_KStrUpdateIniFile;
		
		TCHAR TStrTestFile[1024];
		TCHAR TStrLocal[1024];
		
		lstrcpy(TStrTestFile,StrTestFile);
		lstrcpy(TStrLocal,StrLocal);

	
		StrLog.Format(L"begin download serverfile %s to local %s\n",StrTestFile,StrLocal);
		_LogWithTime(StrLog);

		int32 iResult = HttpDownLoad(TStrTestFile,TStrLocal,this);
		
		StrLog.Format(L"end download serverfile %s to local %s\n",StrTestFile,StrLocal);
		_LogWithTime(StrLog);

		if (iResult != -1 && iResult != -2)
		{
			bFoundFastAddress = true;
			//测试结果,目前只要满足能下载到文件就行.
			m_StrFastestServer = m_aHostAddressList[i];
			StrLog.Format(L"test speed complete,find the fastest server: %s\n",m_StrFastestServer);
			_LogWithTime(StrLog);
			break;
		}
	}
	if ( !bFoundFastAddress )
	{
		_LogWithTime(L"test speed complete,no server aviliable\n");		
		m_OwnerWnd.PostMessage (UM_AutoUpdate_TestSpeedSuccess,0,(LPARAM)&m_aHostAddressList);	
	}
	//通知MainFrame,已经测试好了.
	else
	{
		// 下载Update.ini 完成后,测试服务器成功.顺便把UpdateLog.txt 下下来.这个文件跟Update.ini 是同时要更新的
		StrLog.Format(L"try download UPDATELOG.TXT !!!");
		_LogWithTime(StrLog);
		
		CString StrPathServer =  m_StrFastestServer + L"/updatelog.txt";
		CString StrPathLocal  =  m_KStrUpdateLog;

		TCHAR TStrPathServer[1024];
		TCHAR TStrPathLocal[1024];

		lstrcpy(TStrPathServer,StrPathServer);
		lstrcpy(TStrPathLocal,StrPathLocal);

		int32 iResult = HttpDownLoad(TStrPathServer,TStrPathLocal,this);

		if (iResult != -1 && iResult != -2)
		{
			StrLog.Format(L"download UPDATELOG.TXT successful!!");
		}
		else
		{
			StrLog.Format(L"download UPDATELOG.TXT failed!!");
		}
		m_OwnerWnd.PostMessage (UM_AutoUpdate_TestSpeedSuccess,1,0);	
	}
}

void CAutoUpdate::OnTestSpeedSuccess()
{	
	//////////////////////////////////////////////////////////////////////////
	//	1: 测速线程函数完毕.得到最快的服务器地址
	//  2: 已经从以上地址下载得到update.ini.
	//  3: 解析 .ini 文件

	CString StrLog;
	StrLog.Format(L"already find the fastest server:%s, begin translate local update.ini file,select which file needs download\n\n",m_StrFastestServer);
	_LogWithTime(StrLog); 

	CString StrFileNums;
	// 得到文件个数:

	GetPrivateProfileStringGGTong(L"File_Nums",L"FileNumber",L"",StrFileNums,m_KStrUpdateIniFile);
	
	StrLog = L"Get File Numbers: " + StrFileNums + L"!\n";	
	_LogWithTime(StrLog);

	int32 iFileNums = _ttoi(StrFileNums);


	for (int32 i = 0;i<iFileNums;i++)
	{
		CString StrLog;
		StrLog.Format(L"begin read file %d\n",i);
		_LogWithTime(StrLog);
		
		CString StrAppName;
		CString StrFileName,StrPathServer,/*StrPathLocalTemp*/StrPathLocalDes,StrVersionServer,StrDegree,StrSilence;
		T_UpdataInfo UpdateInfo;
	
		StrAppName.Format(L"File_%d",i);

		GetPrivateProfileStringGGTong(StrAppName,L"FileName",L"",StrFileName,m_KStrUpdateIniFile);
		GetPrivateProfileStringGGTong(StrAppName,L"PathServer",L"",StrPathServer,m_KStrUpdateIniFile);
		//GetPrivateProfileStringGGTong(StrAppName,L"PathLocalTemp",L"",StrPathLocalTemp,m_KStrUpdateIniFile);
		GetPrivateProfileStringGGTong(StrAppName,L"PathLocalDes",L"",StrPathLocalDes,m_KStrUpdateIniFile);
		GetPrivateProfileStringGGTong(StrAppName,L"Version",L"",StrVersionServer,m_KStrUpdateIniFile);
		//GetPrivateProfileStringGGTong(StrAppName,L"Degree",L"",StrDegree,m_KStrUpdateIniFile);
		//GetPrivateProfileStringGGTong(StrAppName,L"Silence",L"",StrSilence,m_KStrUpdateIniFile);
		
		//StrLog.Format(L"\n FileName:%s;\n PathServer:%s;\n PathLocalTemp:%s;\n PathLocalDes:%s;\n VersionServer:%s;\n Degree:%s;\n Silence:%s;\n",StrFileName,StrPathServer,StrPathLocalTemp,StrPathLocalDes,StrVersionServer,StrDegree,StrSilence);
		StrLog.Format(L"\n FileName:%s;\n PathServer:%s;\n PathLocalDes:%s;\n VersionServer:%s;\n ",StrFileName,StrPathServer,StrPathLocalDes,StrVersionServer);
		_LogWithTime(StrLog);

		if( (StrFileName.GetLength()>0 && StrPathServer.GetLength()>0  && StrVersionServer.GetLength() >0 ) )
		{	
			//... 是Dll ,Exe 等文件,用GetFileVersion()
			if ( (StrFileName.Find(L".exe") != -1) || (StrFileName.Find(L".dll") != -1) )
			{
				CString StrVersionLocal = GetFileVersion(StrPathLocalDes);

				CString StrLog;
				StrLog.Format(L"Local file version:%s\n",StrVersionLocal);
				_LogWithTime(StrLog);

				if (StrVersionLocal.Compare(StrVersionServer) < 0)
				{
					_LogWithTime(L"Need Update current file\n");
					
					// 需要更新
					UpdateInfo.StrPathServer	 = m_StrFastestServer + StrPathServer;
					UpdateInfo.StrPathLocalTemp  = L"./temp/"+StrFileName;
					UpdateInfo.StrPathLocalDes   = StrPathLocalDes;
					
					m_aUpdateInfo.Add(UpdateInfo);

// 					// 比较强制升级的最低版本， 是否需要强制升级
// 					CString		 StrVersionAtLeast;		//最低版本
// 					int32 iPos = StrDegree.Find(L":");
// 					StrVersionAtLeast = StrDegree.Left(iPos); 
// 					
// 					if (StrVersionLocal.Compare(StrVersionAtLeast)<0)
// 					{
// 						_LogWithTime(L"Need [force update]\n");
// 						m_bForceUpdate = true;
//					}										
				}
				else
				{
					_LogWithTime(L"no need update current file\n");
				}
			}
			else
			{
				// 预留 .xml .ini .wsp .cfm .vmg 等文本文件
				 
				NULL;
				/*
				time_t tLocalTime = GetFileLMTime(StrPathLocalDes);
				time_t tServeTime = _ttol(StrVersionServer);
				
				CString StrLog;
				StrLog.Format(L"local file version :%d\n",tLocalTime);
				_LogWithTime(StrLog);

				// 普通文本文件,只要本地时间和服务器时间不一致就下载:
				// 1: 服务器有更新,导致时间不一致								======> 需要下载
				// 2: 本地文件被用户手动修改,导致时间不一致(可能导致文件无效)	======> 需要下载
				if (tLocalTime != tServeTime)			
				{
					_LogWithTime(L"need update current file\n");

					// 需要更新
					UpdateInfo.StrPathServer     = m_StrFastestServer +StrPathServer;
					UpdateInfo.StrPathLocalTemp  = StrPathLocalTemp;
					UpdateInfo.StrPathLocalDes   = StrPathLocalDes;
					
					if (_ttoi(StrSilence) != 0)		// 静默升级文件	
					{
						m_aSilenceUpdateInfo.Add(UpdateInfo);
					}
					else	// 需要提示用户的升级文件
					{
						m_aUpdateInfo.Add(UpdateInfo);

						// 比较强制升级的最低版本， 是否需要强制升级
						CString		 StrVersionAtLeast;		
						int32 iPos = StrDegree.Find(L":");
						StrVersionAtLeast = StrDegree.Left(iPos); 
						
						time_t tTimeAtLeast = _ttol(StrVersionAtLeast);
						
						if (tLocalTime < tTimeAtLeast)
						{
							_LogWithTime(L"need [force update]\n");
							m_bForceUpdate = true;
						}
						
					}
					
				}
				else
				{
				   _LogWithTime(L"no need update current file\n");
				}
				*/
			}				
		}
		else
		{
			// 这个节点读取出错,忽略这个节点
			_LogWithTime(L"read this section failed,continue read next\n");

		}
	}
	
	// 下载静默升级的文件
	// DownloadSilenceFiles();
	
	// 提示用户下载升级文件
	if (m_aUpdateInfo.GetSize()>0)
	{	 		
// 		if (1/*m_bForceUpdate*/)
// 		{
// 			_LogWithTime(L"begin force update,tell user to choose.\n");
// 			//... 如果为真,强制更新
// 			if ( IDNO == AfxMessageBox(L"程序已有重要更新,请更新完毕后重新进入系统!",MB_YESNO))
// 			{
// 				_LogWithTime(L"user cancel force update,PostQuitMessage(0) .\n");
// 				//...结束应用程序
// 				PostQuitMessage(0);
// 			}
// 			else
// 			{
// 				//... 下载任务对话框	
// 				
// 				if ( !m_pDlgSysUpdate ) 
// 				{	_LogWithTime(L"user choose force update,go to DlgSysUpdate \n");
// 					m_pDlgSysUpdate = new CDlgSysUpdate;
// 					m_pDlgSysUpdate->GetInitialData(m_aUpdateInfo);
// 					m_pDlgSysUpdate->Create(IDD_DIALOG_UPDATE,&m_OwnerWnd); 
// 					m_pDlgSysUpdate->ShowWindow(SW_SHOW);							
// 				}
// 			}
// 		}
// 		else
// 		{
// 			NULL;
// 			/*
// 			//... 根据用户选择,判断是否更新
// 			_LogWithTime(L"normal update,tell user to choose .\n");
// 
// 			if ( IDYES == AfxMessageBox(L"程序已有更新,是否下载最新版本?",MB_YESNO))
// 			{
// 				//... 下载任务对话框
// 				if ( !m_pDlgSysUpdate )
// 				{	
// 					_LogWithTime(L"user choose normal update,go to DlgSysUpdate .\n");
// 
// 					m_pDlgSysUpdate = new CDlgSysUpdate;
// 					m_pDlgSysUpdate->GetInitialData(m_aUpdateInfo);
// 					m_pDlgSysUpdate->Create(IDD_DIALOG_UPDATE,&m_OwnerWnd); 
// 					m_pDlgSysUpdate->ShowWindow(SW_SHOW);				
// 				}
// 			}
// 			else
// 			{
// 				// ...非强制更新,用户选择不更新
// 				_LogWithTime(L"user cancel normal update, go to GGtong.exe .\n");
// 				NULL;
// 			}*/				
// 		}




		//////////////////////////////////////////////////////////////////////////
		//	fangz 1101# Modify 	
		
		if ( !m_pDlgSysUpdate )
		{	
			_LogWithTime(L"user choose normal update,go to DlgSysUpdate .\n");
			
			m_pDlgSysUpdate = new CDlgSysUpdate;
			m_pDlgSysUpdate->GetInitialData(m_aUpdateInfo);
			m_pDlgSysUpdate->Create(IDD_DIALOG_UPDATE,&m_OwnerWnd); 
			m_pDlgSysUpdate->ShowWindow(SW_SHOW);
		}
	}
	else
	{
		if (!m_bAutoUpdate)
		{
			AfxMessageBox(L"系统已经是最新版本,无需更新!");
		}
		
	}
}	 

void CAutoUpdate::DownloadSilenceFiles()
{
	_LogWithTime(L"~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Begin  CAutoUpdate::DownloadSilenceFiles()~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
	//	AfxMessageBox(L"Silence!");
	// 启动线程函数, 下载所有的静默升级文件		
	CWinThread* pThread =  AfxBeginThread(CAutoUpdate::DownloadSilenceFilesThreadProc,this);	
	sDownloadSilenceThreadHandle = pThread->m_hThread;

}

UINT CAutoUpdate::DownloadSilenceFilesThreadProc( LPVOID pParam )
{
	CAutoUpdate* pAutoUpdate = (CAutoUpdate*)pParam;
	ASSERT(NULL != pAutoUpdate);

	pAutoUpdate->DownloadSilenceFilesThreadProc();
	return 0;
}

void CAutoUpdate::DownloadSilenceFilesThreadProc()
{
	CString StrLog;
	m_aFilesfromServer.RemoveAll();
	if (0 == m_StrFastestServer.GetLength())
	{
		m_StrFastestServer = KStrDefaultHttpServer; 
	}
	
	for (int32 i =0 ; i < KiUpdateFilesCounts; i++)
	{
		if ( gbStopDownloadThreadProc )
		{
			_LogWithTime(L"MainFrame Close,Exit DownLoadThread!");
			sDownloadSilenceThreadHandle	= NULL;
			gbStopDownloadThreadProc	    = false;
			break;
		}
	
		CString StrPathServerLogic = s_astUpdateFilesInfo[i].m_StrFilePath.Right(s_astUpdateFilesInfo[i].m_StrFilePath.GetLength()-1);
		
		CString StrPathServer = m_StrFastestServer + StrPathServerLogic;
		CString StrPathLocalTemp  = L"./temp/" + s_astUpdateFilesInfo[i].m_StrFileName;
		
		TCHAR TStrPathServer[1024];
		TCHAR TStrPathLocalTemp[1024];
		
		lstrcpy(TStrPathServer,StrPathServer);
		lstrcpy(TStrPathLocalTemp,StrPathLocalTemp);
		
		StrLog.Format(L"\n File%d: \n PathServer: %s \n PathLocalTemp: %s",i,StrPathServer,StrPathLocalTemp);
		_LogWithTime(StrLog);
		//对服务器文件属性与本地文件属性进行比较.判断是否需要下载
		
		CInternetSession     InternetSessino;
			
		DWORD         dwFlag = INTERNET_FLAG_TRANSFER_BINARY|INTERNET_FLAG_DONT_CACHE|INTERNET_FLAG_RELOAD ;
		CHttpFile     * pF = (CHttpFile*)InternetSessino.OpenURL(StrPathServer, 1, dwFlag);
		ASSERT(pF);
		if (!pF)
		{
			AfxThrowInternetException(1);
		}
		
		// 得到服务器文件最后修改时间
		SYSTEMTIME	stLastModify;
		bool32 bValid = pF->QueryInfo(HTTP_QUERY_LAST_MODIFIED,&stLastModify);
		
		if (!bValid)
		{	
			StrLog.Format(L"get %s infomation from server failed,continue next file... ",StrPathServer);
			_LogWithTime(StrLog);
			continue;

		}
		CTime ctmServer(stLastModify);
		time_t tmServer = ctmServer.GetTime();
			
		// 得到本地文件最后修改时间
		
		time_t tmLocal = GetFileLMTime(s_astUpdateFilesInfo[i].m_StrFilePath);
		
		StrLog.Format(L"\n ServerFileTime:%d , LocalTime: %d",tmServer,tmLocal);
		_LogWithTime(StrLog);
		if ( tmServer == tmLocal)
		{
			_LogWithTime(L"\n No need to download this file,continue next... \n");
			continue;
		}		
		int32 iResult = HttpDownLoad(TStrPathServer,TStrPathLocalTemp,this);
		
		if (iResult!= -1 && iResult != -2)
		{
			// 下载成功
			_LogWithTime(L"\n Download this file Successful\n");
			m_aFilesfromServer.Add(s_astUpdateFilesInfo[i]);
			// 下载成功还不能向主线程发消息,要等到文件从临时文件夹拷贝到目标目录成功后才能发消息 
			//  m_OwnerWnd.SendMessage(UM_AutoUpdate_SilenceFileUpdateSuccess, (WPARAM)&s_astUpdateFilesInfo[i].m_StrFilePath, 0);
		}
		else
		{
			// 下载失败， 不重要
			_LogWithTime(L"\n Download this file Failed\n");			
			NULL;
		}
	}
	CopySilenceFilesToDestination();

	sDownloadSilenceThreadHandle	= NULL;
	gbStopDownloadThreadProc		= false;
}
void CAutoUpdate::TryToUpdateFiles()
{
	// 从服务器下载需要更新的文件		
	DownloadSilenceFiles();
}
void CAutoUpdate::CopySilenceFilesToDestination()
{
	// 1: 已经将服务器文件下到本地临时文件夹
	// 2: 将temp 目录文件更新到目标地址

	CString StrLog;
	if ( 0 == m_aFilesfromServer.GetSize())
	{
		// 如果没有需要更新的文件,重新设定定时器,退出即可;
		_LogWithTime(L"\n No files to update!!! \n");
		_LogWithTime(L"\n ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ End Update SilencFile~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
		CMainFrame * pMainFrame = (CMainFrame*)AfxGetMainWnd();
		pMainFrame->SetTimer(UPDATEFILESTIMER,1000*60*60*3,NULL);	// 3小时更新一次
		return;
	}
	
	_LogWithTime(L"\n ------- Move files from LocalTemp to Destination -------\n");
	
	for (int32 i = 0 ; i< m_aFilesfromServer.GetSize(); i++)
	{
		
		CFileFind filefind;
		if (filefind.FindFile(m_aFilesfromServer[i].m_StrFilePath))
		{
			StrLog.Format(L"%s exist,updateing...",m_aFilesfromServer[i].m_StrFilePath);
			_LogWithTime(StrLog);
			// 文件存在.更新文件
			CString    StrFilePath = m_aFilesfromServer[i].m_StrFilePath;
			HANDLE     h_mfile     = CreateFile(StrFilePath ,GENERIC_READ|GENERIC_WRITE,     
				FILE_SHARE_READ|FILE_SHARE_DELETE,NULL, OPEN_EXISTING,   FILE_FLAG_BACKUP_SEMANTICS,   NULL);  				
			StrLog.Format(L"\n FilePath:%s",StrFilePath);
			_LogWithTime(StrLog);
			
			if ( h_mfile == INVALID_HANDLE_VALUE)
			{				
				// 文件可用,更新!
				
				CString StrPathLocalTemp  = L"./temp/" + m_aFilesfromServer[i].m_StrFileName;						
				// 将文件从临时目录拷贝到目标目录
				// 拷贝文件时,如果文件存在,则覆盖
				
				TCHAR TStrPathLocal[1024];
				lstrcpy(TStrPathLocal,m_aFilesfromServer[i].m_StrFilePath);
				_tcheck_if_mkdir(TStrPathLocal);
				
				bool32 bSuccess = ::CopyFile(StrPathLocalTemp,m_aFilesfromServer[i].m_StrFilePath,FALSE);   
				
				StrLog.Format(L"\n CopyFile From : %s To: %s",StrPathLocalTemp,m_aFilesfromServer[i].m_StrFilePath);
				_LogWithTime(StrLog);
				if (bSuccess)
				{
					// 发消息
					m_OwnerWnd.SendMessage(UM_AutoUpdate_SilenceFileUpdateSuccess, (WPARAM)&m_aFilesfromServer[i].m_StrFilePath, 0);
					_LogWithTime(L"Copy Successfully!!");	
					
					CFile::Remove(StrPathLocalTemp);
					m_aFilesfromServer.RemoveAt(i);				
					i -= 1;
				}
				else
				{			
					DWORD dwError = GetLastError();
					_LogWithTime(L"Copy Failed!!");
				}
				CloseHandle(h_mfile);		
			}
			else
			{				
				// 文件不可用.跳过!
				_LogWithTime(L"File is in use now!!!!!!!!!! continue update next file...");
				continue;				
			}
		}
		else
		{
			// 文件不存在,拷贝文件			
			CString StrPathLocalTemp  = L"./temp/" + m_aFilesfromServer[i].m_StrFileName;						
			
			TCHAR TStrPathLocal[1024];
			lstrcpy(TStrPathLocal,m_aFilesfromServer[i].m_StrFilePath);
			_tcheck_if_mkdir(TStrPathLocal);
			
			bool32 bSuccess = ::CopyFile(StrPathLocalTemp,m_aFilesfromServer[i].m_StrFilePath,FALSE);   
			
			StrLog.Format(L"\n File do not exist,CopyFile From : %s To: %s",StrPathLocalTemp,m_aFilesfromServer[i].m_StrFilePath);
			_LogWithTime(StrLog);
			
			if (bSuccess)
			{
				m_OwnerWnd.SendMessage(UM_AutoUpdate_SilenceFileUpdateSuccess, (WPARAM)&m_aFilesfromServer[i].m_StrFilePath, 0);
				_LogWithTime(L"Copy Successfully!!");	
				CFile::Remove(StrPathLocalTemp);				
				m_aFilesfromServer.RemoveAt(i);				
				i -= 1;
			}
		}				
	}
}
bool CAutoUpdate::OnProgress(int iProcess,int iMax)
{
	// 下载过程中,检测MainFrame 是否退出.
	if ( gbStopDownloadThreadProc )
	{
		return false;
	}
	return true;
}