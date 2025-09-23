#include "StdAfx.h"
#include "atlconv.h"
#include "WspManager.h"
#include "PathFactory.h"
#include "ShareFun.h"
#include "XmlShare.h"
#include "tinyxml.h"
#include "MPIChildFrame.h"
#include "PluginFuncRight.h"
#include "coding.h"
#include "GGTongView.h"
#include <string>
using std::string;


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
// 最后每个用户打开的工作区记录 xml
static const char  *KStrLastUserSelCfmFileName = "UserDefautCfm.xml";
static const char  *KStrLastUserSelCfmKey = "record";
static const char  *KStrLastUserSelCfmAttriUserName = "user";
static const char  *KStrLastUserSelCfmAttriCfmXmlName = "cfm";

static const CString  KStrLastUserSelDefaultUserName = _T("");	//  默认名称项
static CString  KStrLastUserSelDefaultWspXmlName = _T("首页");	//  默认名称项 - 会被调整为智能选股名称

// xml
static const char * KStrElementAttriFileShowName	= "FileShowName";

static const char *KStrEleAttriSecret = "secret";	
static const char *KStrEleAttriUpdateMode  = "updateMode";
static const char *KStrEleAttriLockSplit = "lockSplit";
static const char *KStrEleAttriNeedServerAuth = "needAuth";
static const char *KStrEleAttriSystemShow = "system";
static const char *KStrEleAttriFromSystem = "fromSystem";	// 从系统文件复制的
static const char *KStrEleAttriNeedReload = "needReload";	// 再次打开cfm是否需要重新加载


//////////////////////////////////////////////////////////////////////////
CCfmManager::CCfmManager()
{
	// 强制将 KStrLastUserSelDefaultWspXmlName 同名
	CString StrDefaultCfmName;
	if ( GetSystemDefaultCfmName(ESDC_HomePage, StrDefaultCfmName) )
	{
		KStrLastUserSelDefaultWspXmlName = StrDefaultCfmName;
	}
	LoadPublicLastCfmList();
}

CCfmManager::~CCfmManager()
{
	SavePublicLastCfmList();
}

void CCfmManager::Initialize( const CString &StrUserName )
{
	ASSERT( !StrUserName.IsEmpty() );
	
	m_mapCfmInfoSystem.clear();
	m_mapCfmInfoUser.clear();
	m_StrUserName = StrUserName;

	// 确保私人文件夹创建
	CString StrPrivateWspPath = CPathFactory::GetPrivateWorkspacePath(StrUserName);
	_tcheck_if_mkdir(StrPrivateWspPath.GetBuffer(MAX_PATH));
	StrPrivateWspPath.ReleaseBuffer();

	// 查找该用户的所有工作区信息
	RefreshCfm();

	//  是否建立临时文件，运行过程中自动保存到临时目录，程序启动时，清除临时目录
	//	最后提示是否保存工作区，如果保存，则将所有存在的临时文件转存到私有目录
	//	现在自动保存所有的更改

	// 初始化Last wsp
	if ( GetUserLastSelCfm(m_cfmUserDefault, StrUserName) )	// 内部设定默认工作页面
	{
		SetUserLastSelCfm(m_cfmUserDefault.m_StrXmlName, StrUserName);	// 保存一下
	}
	else
	{
		AddNewUserCfm(KStrLastUserSelDefaultWspXmlName);	// 创建一个默认的cfm
		QueryUserCfm(KStrLastUserSelDefaultWspXmlName, m_cfmUserDefault);	// 初始化默认
		SetUserLastSelCfm(m_cfmUserDefault.m_StrXmlName, StrUserName);	// 保存一下
	}
}

void CCfmManager::RefreshCfm()
{
	ASSERT( !m_StrUserName.IsEmpty() );
	
	m_mapCfmInfoSystem.clear();
	m_mapCfmInfoUser.clear();
	
	// 查找该用户的所有工作区信息
	CfmInfoMap cfmInfos;
	// 此时初始化的wsp可能SystenHas不准确，先初始化system
	ScanUserAllCfm(cfmInfos, m_mapCfmInfoSystem, m_mapCfmInfoUser, m_StrUserName);
	
#ifdef _DEBUG
	{
		for ( CfmInfoMap::const_iterator it = m_mapCfmInfoSystem.begin() ; it != m_mapCfmInfoSystem.end() ; it++ )
		{
			ASSERT( !it->second.m_bInUserPrivateDir && it->second.m_bSystemDirHas );
		}
	}
#endif //_DEBUG
	
#ifdef _DEBUG
	// user信息可能不准确 再初始化user
	{
		for (  CfmInfoMap::iterator it = m_mapCfmInfoUser.begin() ; it != m_mapCfmInfoUser.end() ; it++  )
		{
			ASSERT( it->second.m_bInUserPrivateDir );
			if ( it->second.m_bSystemDirHas )
			{
				ASSERT( CheckCfmExist(it->first, m_mapCfmInfoSystem, NULL) );
			}
			else
			{
				ASSERT( !CheckCfmExist(it->first, m_mapCfmInfoSystem, NULL) );
			}
		}
	}
#endif	//_DEBUG
	
	
	// TODO 工作页面
	// 共有与私有可能会脱轨，可能会存在从共有过来的但是现在共有没有了，从而现在被扫描为私有的
}

bool32 CCfmManager::IsInitialized() const
{
	return !m_StrUserName.IsEmpty();
}

/*
	error 437: (Warning -- Passing struct 'CStringT' to ellipsis)
*/
//lint --e{ 437}
void CCfmManager::ScanUserAllCfm( OUT CfmInfoMap &cfmInfos, OUT CfmInfoMap &cfmInfoSys, OUT CfmInfoMap &cfmInfoUser, LPCTSTR pszUserName /*= NULL*/ ) const
{
	// 查找所有属于该用户的工作区，用户目录下的工作区覆盖同xml名的系统工作区
	cfmInfos.clear();
	cfmInfoSys.clear();
	cfmInfoUser.clear();

	if ( NULL == pszUserName )
	{
		pszUserName = m_StrUserName;	// 使用保存的当前用户
	}

	// 有可能是空的用户~， 空用户仅加载系统的

	// 先加载系统的, 系统的是实时扫描， 总是使用最新扫描的
	CFileFind file;
	CString	StrDir = CPathFactory::GetPublicWorkspacePath();
	StrDir.Replace(_T('\\'), _T('/'));
	StrDir += _T("/*");
	StrDir += GetProperFileExt(false);
	bool32 bContinue = file.FindFile(StrDir);
	while ( bContinue )
	{
		bContinue = file.FindNextFile();

		T_CfmFileInfo cfmInfo;
		if ( ReadCfmInfoFromFile(file.GetFilePath(), cfmInfo, true) )
		{
			int iCanAdd = CheckCfmCanAdd(cfmInfo, cfmInfos, true);
			ASSERT( iCanAdd >= 0 );
			if ( iCanAdd > 0 )
			{
				TRACE(_T("系统cfm加载过程中发现名称冲突, 重命名为: %s\r\n"), cfmInfo.m_StrXmlName);
			}
			cfmInfos[ cfmInfo.m_StrXmlName ] = cfmInfo;
			cfmInfoSys[ cfmInfo.m_StrXmlName ] = cfmInfo;
		}
	}

	// 在看用户的能不能加载
	if ( pszUserName != NULL && pszUserName[0] != _T('\0') )
	{
		// 先尝试.cfm系统格式以及以前保留的格式的文件, 只有这种格式的才会认为是系统文件来的
		StrDir = CPathFactory::GetPrivateWorkspacePath(CString(pszUserName));
		StrDir.Replace(_T('\\'), _T('/'));
		StrDir += _T("/*");
		StrDir += GetProperFileExt(false);

		bContinue = file.FindFile(StrDir);
		while ( bContinue )
		{
			bContinue = file.FindNextFile();

			T_CfmFileInfo cfmInfo;
			if ( ReadCfmInfoFromFile(file.GetFilePath(), cfmInfo) )
			{
				// 此时的systemDirHas与inPrivate可能会判断失误，不过由于只在登录前才会出错，所以没有问题
				// 由于用户的可以覆盖系统的，所以命名只需要与用户自己的检查就可以了
				int iCanAdd = CheckCfmCanAdd(cfmInfo, cfmInfoUser, true);
				ASSERT( iCanAdd >= 0 );
				if ( iCanAdd > 0 )
				{
					TRACE(_T("用户目录下系统[%s]wsp加载过程中发现名称冲突, 重命名为: %s\r\n"), pszUserName, cfmInfo.m_StrXmlName);
				}
				// 此时user信息system has可能不准确, 更新下
				if ( !cfmInfo.m_bSystemDirHas && cfmInfoSys.count(cfmInfo.m_StrXmlName) > 0 )
				{
					ASSERT( cfmInfo.m_bInUserPrivateDir );
					cfmInfo.m_bSystemDirHas = true;
				}
				cfmInfos[ cfmInfo.m_StrXmlName ] = cfmInfo;	// 覆盖或者新增
				cfmInfoUser[ cfmInfo.m_StrXmlName ] = cfmInfo;
			}
		}

		// 再尝试.cfu用户特有文件
		StrDir = CPathFactory::GetPrivateWorkspacePath(CString(pszUserName));
		StrDir.Replace(_T('\\'), _T('/'));
		StrDir += _T("/*");
		StrDir += GetProperFileExt(true);

		bContinue = file.FindFile(StrDir);
		while ( bContinue )
		{
			bContinue = file.FindNextFile();

			T_CfmFileInfo cfmInfo;
			if ( ReadCfmInfoFromFile(file.GetFilePath(), cfmInfo) )
			{
				// 这里出现的是纯用户工作页面，就算系统有同名也不会处理成系统的
				// ASSERT( !cfmInfo.m_bSystemDirHas );
				cfmInfo.m_bSystemDirHas = false;
				int iCanAdd = CheckCfmCanAdd(cfmInfo, cfmInfos, true);	// 与整个所有工作页面名称冲突
				ASSERT( iCanAdd >= 0 );
				if ( iCanAdd > 0 )
				{
					TRACE(_T("用户[%s]wsp加载过程中发现名称冲突, 重命名为: %s\r\n"), pszUserName, cfmInfo.m_StrXmlName);
				}

				cfmInfos[ cfmInfo.m_StrXmlName ] = cfmInfo;	// 覆盖或者新增
				cfmInfoUser[ cfmInfo.m_StrXmlName ] = cfmInfo;
			}
		}
	}
}

bool32 CCfmManager::ReadCfmInfoFromFile( const CString &StrFilePathOrg, OUT T_CfmFileInfo &cfmInfo, bool32 bSureSysteFile/* = false*/ ) const
{
	// 得到工作区文件中描述的XMLData属性 version xml_name readonly
	if ( StrFilePathOrg.IsEmpty() )
	{
		return false;
	}
	CString StrFilePath = StrFilePathOrg;

	cfmInfo.m_StrFilePath = StrFilePath;
	cfmInfo.m_StrFilePath.Replace(_T('\\'), _T('/'));
	int iFileNameStart = cfmInfo.m_StrFilePath.ReverseFind(_T('/'));
	if ( iFileNameStart < 0 )
	{
		iFileNameStart = -1;
	}
	cfmInfo.m_StrFileName = cfmInfo.m_StrFilePath.Mid(iFileNameStart+1);
	cfmInfo.m_StrXmlName = cfmInfo.m_StrFileName;
	{
		int iPos = cfmInfo.m_StrXmlName.ReverseFind(_T('.'));
		if ( iPos > 0 )
		{
			cfmInfo.m_StrXmlName = cfmInfo.m_StrXmlName.Left(iPos);
		}
	}
	cfmInfo.m_StrXmlName.TrimLeft();
	cfmInfo.m_StrXmlName.TrimRight();
	cfmInfo.m_StrFileVersion.Empty();
	cfmInfo.m_bReadOnly = false;
	cfmInfo.m_bSystemDirHas = false;
	cfmInfo.m_bInUserPrivateDir = true;
	cfmInfo.m_iUpdateMode = 0;
	cfmInfo.m_bSecret = false;
	cfmInfo.m_bLockSplit = false;
	cfmInfo.m_bNeedServerAuth = false;

	// 先判断是不是同一个文件，在看是否已经初始化，如果已经初始化，从用户的系统列表中找一下是否有同xml名的系统wsp
	//    因为系统列表无论什么用户是相同的，所以不用区分用户了
	// 如还不确定就不需要判断了， 已经不能快速判断，而且也不需要判断
	if ( !bSureSysteFile )
	{
		CFileFind fileFind;
		CString StrSysWspPath = CPathFactory::GetPublicWorkspacePath();
		StrSysWspPath.Replace(_T('\\'), _T('/'));
		StrSysWspPath += cfmInfo.m_StrFileName;
		if (fileFind.FindFile(StrSysWspPath))	// 系统目录下是否有该工作区文件？？
		{
			fileFind.FindNextFile();
			StrSysWspPath  = fileFind.GetFilePath();	// 保存具体文件全名

			cfmInfo.m_bSystemDirHas = true;		// 系统文件有了
			cfmInfo.m_bFromSystem = true;

			if ( fileFind.FindFile(StrFilePath) ) // 查找原来的文件
			{
				fileFind.FindNextFile();
				bool32 bSameFile = StrSysWspPath.CompareNoCase(fileFind.GetFilePath()) == 0;	// 相等则为同一个文件
				
				cfmInfo.m_bInUserPrivateDir = !bSameFile;	// 为系统工作区文件，则不在用户私人目录里
				
			}
		}
	}
	else
	{
		cfmInfo.m_bSystemDirHas = true;
		cfmInfo.m_bInUserPrivateDir = false;	// 都确定了，就不比较了
		cfmInfo.m_bFromSystem = true;
	}
	
	TiXmlDocument MyDocument;
	if ( !LoadTiXmlDoc(MyDocument, StrFilePath) )
	{
		return false;
	}
	
	// <XMLDATA>
	TiXmlElement* pRootElement = MyDocument.RootElement();
	
	if ( NULL == pRootElement)
	{
		return false;
	}
	
	// 名称
	if ( !cfmInfo.m_bSystemDirHas && cfmInfo.m_bInUserPrivateDir )
	{
		// 在私有目录里的wsp文件再从用户的system wsp里面比对一下xml名称，看是否有相同xml名称的，如有，视为其模板
		if ( m_mapCfmInfoSystem.count(cfmInfo.m_StrXmlName) > 0 )
		{
			cfmInfo.m_bSystemDirHas = true;
			cfmInfo.m_bFromSystem = true;
		}
	}

	TCHAR awcBuf[1024];
	// version
	const char* StrFileVersion = pRootElement->Attribute(GetXmlRootElementAttrVersion());
	if ( NULL == StrFileVersion )
	{
		return false;
	}
	MultiCharCoding2Unicode(EMCCUtf8, StrFileVersion, -1, awcBuf, sizeof(awcBuf) / sizeof(wchar_t));
	cfmInfo.m_StrFileVersion = awcBuf;

	// readonly
	const char* StrReadOnly = pRootElement->Attribute(GetXmlRootElementAttrReadOnly());
	if ( NULL != StrReadOnly )
	{
		cfmInfo.m_bReadOnly = atoi(StrReadOnly) != 0;
	}

	// update mode
	const char *StrUpdateMode = pRootElement->Attribute(KStrEleAttriUpdateMode);
	if ( NULL != StrUpdateMode )
	{
		cfmInfo.m_iUpdateMode = atoi(StrUpdateMode);
	}

	// lock split 
	const char *pLockSplit = pRootElement->Attribute(KStrEleAttriLockSplit);
	if ( NULL != pLockSplit )
	{
		cfmInfo.m_bLockSplit = atoi(pLockSplit) != 0;
	}

	// secret
	const char *pIndependent = pRootElement->Attribute(KStrEleAttriSecret);
	if ( NULL != pIndependent )
	{
		cfmInfo.m_bSecret = atoi(pIndependent) != 0;
	}

	// need server auth
	const char *pNeedAuth = pRootElement->Attribute(KStrEleAttriNeedServerAuth);
	if ( NULL != pNeedAuth )
	{
		cfmInfo.m_bNeedServerAuth = atoi(pNeedAuth) != 0;
	}

	// 工具条显示 systemshow
	const char *pSystemShow = pRootElement->Attribute(KStrEleAttriSystemShow);
	if ( NULL != pSystemShow )
	{
		cfmInfo.m_iSystemShow = atol(pSystemShow);
	}

	// 是否从系统cfm过来, 这个属性只要满足运行时或者曾经被设置，则永不能在运行时重置
	const char *pFromSystem = pRootElement->Attribute(KStrEleAttriFromSystem);
	if ( NULL != pFromSystem && !cfmInfo.m_bFromSystem )
	{
		cfmInfo.m_bFromSystem = atol(pFromSystem) != 0;
	}

	// 再次打开cfm是否需要重新加载
	const char *pNeedReload= pRootElement->Attribute(KStrEleAttriNeedReload);
	if ( NULL != pNeedReload && !cfmInfo.m_bNeedReload )
	{
		cfmInfo.m_bNeedReload = atol(pNeedReload) != 0;
	}
	
	return true;
}

void CCfmManager::ScanUserNormalCfm( OUT CfmInfoMap &cfmInfos, LPCTSTR pszUserName /*= NULL*/ ) const
{
	CfmInfoMap cfmsSys, cfmsUser;
	ScanUserAllCfm(cfmInfos, cfmsSys, cfmsUser, pszUserName);
	CStringArray aDelXmls;
	for (CfmInfoMap::const_iterator it = cfmInfos.begin() ; it != cfmInfos.end() ; it++)
	{
		if ( it->second.m_bSecret )
		{
			aDelXmls.Add(it->first);
		}
	}
	for ( int i=0; i < aDelXmls.GetSize() ; i++ )
	{
		cfmInfos.erase(aDelXmls[i]);
	}
}

bool32 CCfmManager::QueryUserCfm( const CString &StrXmlName, OUT T_CfmFileInfo &cfmInfo ) const
{
	ASSERT( !m_StrUserName.IsEmpty() );

	// 先用户， 在系统
	if ( !CheckCfmExist(StrXmlName, m_mapCfmInfoUser, &cfmInfo) )
	{
		return CheckCfmExist(StrXmlName, m_mapCfmInfoSystem, &cfmInfo);
	}
	return true;
}

void CCfmManager::QueryUserSystemCfm( OUT CfmInfoMap &cfmInfos ) const
{
	ASSERT( !m_StrUserName.IsEmpty() );

	// 系统中查找，如果用户中有，则使用用户的取代
	cfmInfos.clear();
	for (CfmInfoMap::const_iterator it = m_mapCfmInfoSystem.begin() ; it != m_mapCfmInfoSystem.end() ; it++)
	{
		CfmInfoMap::const_iterator itUser = m_mapCfmInfoUser.find(it->first);
		if ( itUser != m_mapCfmInfoUser.end() )
		{
			cfmInfos.insert(*itUser);// 用户有，则替代
		}
		else
		{
			cfmInfos.insert(*it);
		}
	}
}

void CCfmManager::QueryUserPrivateCfm( OUT CfmInfoMap &cfmInfos ) const
{
	ASSERT( !m_StrUserName.IsEmpty() );

	// 仅存在与用户的
	cfmInfos.clear();
	for (CfmInfoMap::const_iterator it = m_mapCfmInfoUser.begin() ; it != m_mapCfmInfoUser.end() ; it++)
	{
		CfmInfoMap::const_iterator itSys = m_mapCfmInfoSystem.find(it->first);
		if ( itSys != m_mapCfmInfoSystem.end() || it->second.m_bSystemDirHas )
		{
			// 系统如果有，则不认为是private的
			continue;
		}
		else
		{
			cfmInfos.insert(*it);
		}
	}
}

void CCfmManager::QueryUserAllCfm( OUT CfmInfoMap &cfmInfos ) const
{
	ASSERT( !m_StrUserName.IsEmpty() );
	
	// 仅存在与用户的
	cfmInfos.clear();

	cfmInfos = m_mapCfmInfoSystem;	// 先系统

	for (CfmInfoMap::const_iterator it = m_mapCfmInfoUser.begin() ; it != m_mapCfmInfoUser.end() ; it++)
	{
		cfmInfos[it->first] = it->second;	// 新增或覆盖
	}
}


void CCfmManager::LoadPublicLastCfmList()
{
	// 是否提供默认名称设定？
	// 先从公共目录加载默认配置
	TiXmlDocument tiDoc;
	CString StrDir = CPathFactory::GetPublicConfigPath();	
	StrDir.Replace(_T('\\'), _T('/'));
	StrDir += KStrLastUserSelCfmFileName;	// 为ascii，直接+

	m_mapUserLastSelCfm.clear();

	// 设定最初始的默认帐号对应的名称
	m_mapUserLastSelCfm[ KStrLastUserSelDefaultUserName ] = KStrLastUserSelDefaultWspXmlName;

	// 公共目录加载初始化全局的默认
	if ( LoadTiXmlDoc(tiDoc, StrDir) )
	{
		TiXmlElement *pRoot = (TiXmlElement *)tiDoc.RootElement();
		if ( NULL != pRoot )
		{
			TiXmlElement *pEle = pRoot->FirstChildElement(KStrLastUserSelCfmKey);
			while ( NULL != pEle )
			{
				const char *pUser = pEle->Attribute(KStrLastUserSelCfmAttriUserName);
				const char *pCfm  = pEle->Attribute(KStrLastUserSelCfmAttriCfmXmlName);
				if ( NULL != pUser && NULL != pCfm )
				{
					m_mapUserLastSelCfm[ _A2W(pUser) ] = _A2W(pCfm);
				}
				pEle = pEle->NextSiblingElement(KStrLastUserSelCfmKey);
			}
		}
	}
}

void CCfmManager::SavePublicLastCfmList()
{
	if ( m_mapUserLastSelCfm.size() > 0 && IsInitialized() )
	{
		TiXmlDocument tiDoc;
		TiXmlElement *pRoot = ConstructGGTongAppXmlDocHeader(tiDoc, "CfmManager", NULL, NULL, NULL);
		ASSERT( NULL != pRoot );
		CString StrDir = CPathFactory::GetPublicConfigPath();
		StrDir.Replace(_T('\\'), _T('/'));
		StrDir += KStrLastUserSelCfmFileName;	// 为ascii，直接+

		CString StrSysDefaultCfmName = m_mapUserLastSelCfm[KStrLastUserSelDefaultUserName];	// 所有用户缺省的cfm

		for ( UserLastCfmMap::const_iterator it = m_mapUserLastSelCfm.begin() ; it != m_mapUserLastSelCfm.end() ; it++ )
		{
			// 与全局用户缺省相同的都不保存, 全局名称不保存
			if ( it->second.CompareNoCase(StrSysDefaultCfmName) == 0
				|| it->first.CompareNoCase(KStrLastUserSelDefaultUserName) == 0 )
			{
				// 全保存
				//continue;
			}
			TiXmlElement ele(KStrLastUserSelCfmKey);
			ele.SetAttribute(KStrLastUserSelCfmAttriUserName, _W2A(it->first));
			ele.SetAttribute(KStrLastUserSelCfmAttriCfmXmlName, _W2A(it->second));

			pRoot->InsertEndChild(ele);
		}

		SaveTiXmlDoc(tiDoc, StrDir);
	}
}

/*
	error 437: (Warning -- Passing struct 'CStringT' to ellipsis)
*/
//lint --e{ 437}
bool32 CCfmManager::GetUserLastSelCfm( OUT T_CfmFileInfo &cfmInfo, LPCTSTR pszUserName /*= NULL*/ ) const
{
	if ( NULL == pszUserName )
	{
		pszUserName = m_StrUserName;
	}
	ASSERT( NULL != pszUserName );

	CString StrUserName(pszUserName);
	// 获取该用户所有的wsp(不要特殊工作区)
	CfmInfoMap cfmInfos, cfmSyss, cfmUsers;
	ScanUserAllCfm(cfmInfos, cfmSyss, cfmUsers, StrUserName);	// 包含特殊工作区

	// 确定最后打开工作区文件
	//	1. 读取文件配置对应名称，如果无法找到对应wsp，则检查默认名称项(空名称项是否有对应工作区)
	//  2. 最后写入时间排序所有工作区文件, 找到最近写入的文件
	//  3. 无合适的返回失败, mainframe需要处理好没有任何wsp文件时的情况
	UserLastCfmMap::const_iterator itUserSelCfm = m_mapUserLastSelCfm.find(StrUserName);
	if ( itUserSelCfm != m_mapUserLastSelCfm.end() )
	{
		CfmInfoMap::const_iterator it = cfmInfos.find(itUserSelCfm->second);
		if ( it != cfmInfos.end() )
		{
			cfmInfo = it->second;
			return true;	// 找到了合适的wsp文件(大多数情况下都应该是的)
		}
		// 有可能文件被删除了
		TRACE(_T("用户指定最后cfm不能加载: %s\r\n"), itUserSelCfm->second);
	}

	// 检查默认名称项的wsp是否正常，避免打开不可预知的工作区
	itUserSelCfm = m_mapUserLastSelCfm.find(KStrLastUserSelDefaultUserName);
	if ( itUserSelCfm != m_mapUserLastSelCfm.end() )
	{
		CfmInfoMap::const_iterator it = cfmInfos.find(itUserSelCfm->second);
		if ( it != cfmInfos.end() )
		{
			cfmInfo = it->second;
			TRACE(_T("用户指定最后cfm加载默认项: %s\r\n"), itUserSelCfm->second);
			return true;	// 找到默认加载项，作为正常返回
		}
	}

	// 不尝试时间决定
	// 只有任意加载一个了
	// 检查时间排序项: 先看用户自己目录的，然后在比较系统目录的
// 	CStringArray aSortedTimeWsps;
// 	SortByFileTime(cfmInfos, aSortedTimeWsps);
// 	{
// 		int i = 0;
// 		for ( i=0; i < aSortedTimeWsps.GetSize() ; i++ )
// 		{
// 			CfmInfoMap::const_iterator it = cfmInfos.find(aSortedTimeWsps[i]);
// 			ASSERT( it != cfmInfos.end() );
// 			if ( it != cfmInfos.end() && it->second.m_bInUserPrivateDir )// 私人目录下的优先
// 			{
// 				cfmInfo = it->second;
// 				ASSERT( !cfmInfo.m_bSecret );	// 应当不是特殊工作区
// 				TRACE(_T("用户指定最后cfm加载时间最近项(用户): %s\r\n"), cfmInfo.m_StrXmlName);
// 				return true;	// 
// 			}
// 		}
// 
// 		for ( i=0; i < aSortedTimeWsps.GetSize() ; i++ )
// 		{
// 			CfmInfoMap::const_iterator it = cfmInfos.find(aSortedTimeWsps[i]);
// 			ASSERT( it != cfmInfos.end() );
// 			if ( it != cfmInfos.end() && !it->second.m_bInUserPrivateDir )// 轮到系统目录了，其实应该全部是系统目录了
// 			{
// 				cfmInfo = it->second;
// 				TRACE(_T("用户指定最后cfm加载时间最近项(系统): %s\r\n"), cfmInfo.m_StrXmlName);
// 				return true;	// 
// 			}
// 		}
// 	}

	// 此时如果找不到则证明没有一个可以正常打开的默认工作页面，mainframe需要处理好这种情况
	TRACE(_T("用户无法匹配任何cfm: %s\r\n"), StrUserName);
	ASSERT( 0 );
	return false;
}

void CCfmManager::SetUserLastSelCfm( const CString &StrWspXmlName, const CString &StrUserName )
{
	if ( StrUserName.IsEmpty() )
	{
		return;			// 空用户名是不需要处理的, 而且在登录时，先选择wsp可能会导致这种情况
	}
	ASSERT( StrUserName != KStrLastUserSelDefaultUserName );	// 应当是不出现这个用户名的

	// 假定指定的wsp是一定存在的，避免再次搜索一次
	// 假如这个用户不存在，记录有意义么？？ - 这个应当在登录认证完成时记录
	m_mapUserLastSelCfm[StrUserName] = StrWspXmlName;		// 不管实际这个用户存不存在，都给他记录下来~

	SavePublicLastCfmList();	// 保存
}

void CCfmManager::SortByFileTime( const CfmInfoMap &cfmInfos, OUT CStringArray &aCfmXmlNames )
{
	aCfmXmlNames.RemoveAll();
	CArray<CTime, const CTime &> aTimes;
	aCfmXmlNames.SetSize(0, cfmInfos.size());
	aTimes.SetSize(0, cfmInfos.size());
	for ( CfmInfoMap::const_iterator it = cfmInfos.begin() ; it != cfmInfos.end() ; it++ )
	{
		CFileStatus status;
		if ( CFile::GetStatus(it->second.m_StrFilePath, status) )
		{
			bool32 bInsert = false;
			for ( int i=0; i < aTimes.GetSize() ; i++ )	// 没有时间戳的不在比较范围内
			{
				if ( status.m_mtime > aTimes[i] )
				{
					aTimes.InsertAt(i, status.m_mtime);
					aCfmXmlNames.InsertAt(i, it->first);
					bInsert = true;
					break;
				}
			}
			if ( !bInsert )
			{
				aTimes.Add(status.m_mtime);
				aCfmXmlNames.Add(it->first);
			}
		}
		else
		{
			aCfmXmlNames.Add(it->first);	// 没有时间戳的总是在最后面~~
		}
	}
}


bool32 CCfmManager::AddNewUserCfm( const CString &StrXmlName, bool32 bUserCfm/*=true*/ )
{
	ASSERT( IsInitialized() );
	// 创建一个新的空的cfm
	// TODO 行为需要进一步确定
	T_CfmFileInfo cfmNew;
	cfmNew.m_StrXmlName = cfmNew.m_StrFileName = StrXmlName;
	cfmNew.m_StrFileName += GetProperFileExt(bUserCfm);//_T(".cfm");	// 只要是新建的都是这个名称
	if ( IsNewXmlNameConflict(StrXmlName) > EWCT_CanOverwrite )
	{
		ASSERT( 0 );	// 此时应该前面的流程筛选的 - cfm不能重名
		return false;	// xml名称冲突，无解
	}

	if ( IsNewFileNameConflict(cfmNew.m_StrFileName) )
	{
		ASSERT( 0 );
		return false;	// 文件名称冲突，cfm无解
	}
	
	cfmNew.m_bSystemDirHas = false;	// 
	cfmNew.m_bInUserPrivateDir = true;
	cfmNew.m_bReadOnly = false;
	cfmNew.m_bLockSplit = false;
	cfmNew.m_bSecret = false;
	cfmNew.m_iUpdateMode = 0;
	cfmNew.m_bNeedServerAuth = false;

	cfmNew.m_StrFileVersion = _T("1.0.0.1");		// 新建的版本号是确定还是根据cur来？
	if ( QueryUserCfm(StrXmlName, cfmNew) )
	{
		// 如果有以前的wsp，则继承其属性
		// 如果存在以前的wsp，则由于xml名称相同，所以其 file/version/m_bSystemWspDirHas 应当相同
		ASSERT( !cfmNew.m_bSecret );	// 不应该有隐藏的出现
		DelUserCfm(StrXmlName);	// 删除以前的wsp
	}

	CGGTongDoc* pDoc = (CGGTongDoc*)AfxGetDocument();
	cfmNew.m_bInUserPrivateDir = true;	// 移动到私有目录下
	CString StrPath = CPathFactory::GetPrivateWorkspacePath(pDoc->m_pAbsCenterManager->GetUserName());
	StrPath += cfmNew.m_StrFileName;
	RelativePathToAbsPath(StrPath, cfmNew.m_StrFilePath);		// 变更为私有目录下

	UpdateUserCfm(StrXmlName, cfmNew);

	return true;
}

int CCfmManager::CheckCfmCanAdd( INOUT T_CfmFileInfo &cfmInfo, const CfmInfoMap &infoMap, bool32 bEnableRename /*= false*/ )const
{
	const int cannotAdd = -1;
	const int addedDirect = 0;
	const int addedRename = 1;
	// 添加规则(一般情况):
	//	1. 是否存在同名？
	//	2. 同名的话，是否允许重命名，如果允许，则重命名xml名称 = xml_filename1(?)
	if ( infoMap.count(cfmInfo.m_StrXmlName) )
	{
		if ( bEnableRename )
		{
			CString StrFileHeader = cfmInfo.m_StrFileName;
			CString StrFileTail;
			CString StrNum;
			int iDelPos = StrFileHeader.ReverseFind(_T('.'));
			if ( iDelPos > 0 )
			{
				StrFileHeader = StrFileHeader.Left(iDelPos);
				StrFileTail	  = StrFileHeader.Mid(iDelPos);
			}
			CString StrName;
			if ( cfmInfo.m_bFromSystem )
			{
				StrName = cfmInfo.m_StrXmlName + _T("_") + StrFileHeader;
			}
			else
			{
				StrName = cfmInfo.m_StrXmlName + _T("[用户]");
			}
			CString StrNameBase = StrName;
			int iStart = 1;
			while ( infoMap.count(StrName) )
			{
				// 还重复，则使用数字了
				StrNum.Format(_T("%d"), iStart);
				StrName =  StrNameBase + StrNum;
				iStart++;
			}
			cfmInfo.m_StrXmlName = StrName;	// 换新的名字
			return addedRename;
		}
		return cannotAdd;
	}
	return addedDirect;
}

void CCfmManager::RelativePathToAbsPath( const CString &StrRelative, OUT CString &StrAbs )
{
	// 相对路径转为绝对路径
	StrAbs.Empty();
	if ( StrRelative.IsEmpty() )
	{
		return;
	}

	CString StrR = StrRelative;
	StrR.Replace(_T('\\'), _T('/'));
	if ( StrR.Find(_T(':')) == 1 )
	{
		StrAbs = StrR;		// 已经是绝对路径
		return;
	}

	// ./xxx 或者 xxx/xxx
	// 找出后面的尾

	CString StrTail;	// 包含前 /
	int iPos = StrR.Find(_T("./"));
	if ( iPos == 0 )
	{
		StrTail = StrR.Mid(iPos+1);
	}
	else if ( StrR == _T(".") )	// 当前目录
	{
		StrTail = _T("/");
	}
	else
	{
		if ( StrR[0] != _T('/') )
		{
			StrTail = _T("/") + StrR;	// /xxx
		}
		else
		{
			StrTail = StrR;		// /xxx
		}
	}

	ASSERT( StrTail[0] == _T('/') );

	CString StrCur;
	_tgetcwd(StrCur.GetBuffer(MAX_PATH), MAX_PATH);
	StrCur.ReleaseBuffer();

	StrCur.Replace(_T('\\'), _T('/'));
	StrCur.TrimRight(_T('/'));
	ASSERT( !StrCur.IsEmpty() );

	StrAbs = StrCur + StrTail;
}

CCfmManager::E_ConflictType CCfmManager::IsNewXmlNameConflict( const CString &StrXmlName ) const
{
	ASSERT( IsInitialized() );

	if ( StrXmlName.IsEmpty() )
	{
		ASSERT( 0 );
		return EWCT_Conflict;	// 空名称是前面出现空名文件的bug。提示一下
	}

	// 保留的关键字
	if ( (StrXmlName.Find(L".wsp")!=-1) 
		|| (StrXmlName.Find(L".cfm")!=-1) 
		|| (StrXmlName.Find(L".vmg")!=-1) 
		|| (StrXmlName.Find(L"默认")!=-1)
		|| (StrXmlName.Find(_T("system")) != -1)
		|| (StrXmlName.Find(AfxGetApp()->m_pszAppName) != -1)
		)
	{
		return EWCT_Conflict;
	}

	// cfm不允许任何同名(不区分大小写)
	if ( m_mapCfmInfoUser.count(StrXmlName) > 0 )
	{
		return EWCT_Conflict;
	}
	
	if ( m_mapCfmInfoSystem.count(StrXmlName) > 0 )
	{
		return EWCT_Conflict;
	}

	return EWCT_None;
}

bool32 CCfmManager::IsNewFileNameConflict( const CString &StrFileName ) const
{
	ASSERT( IsInitialized() );
	// 仅检查下用户目录下的记录
	for ( CfmInfoMap::const_iterator it = m_mapCfmInfoUser.begin() ; it != m_mapCfmInfoUser.end() ; it++ )
	{
		if ( it->second.m_StrFileName.CompareNoCase(StrFileName) == 0 )
		{
			return true;
		}
	}
	return false;
}

void CCfmManager::DelUserCfm( const CString &StrXmlName, bool bNoCheckRestore)
{
	DWORD dwRights = GetCfmUserRights(StrXmlName);
	if( bNoCheckRestore )
	{
		//不需要检查
	}
	else //需要检查ECUR_Restore
	{
		if( (dwRights&ECUR_Delete) == 0 && (dwRights&ECUR_Restore) == 0 )	// 无法删除或者还原
		{
			ASSERT( 0 );
			return;		// 当前wsp不能删除，系统-用户wsp就算删除了只是还原而已
		}
	}
	
	CfmInfoMap::iterator it = m_mapCfmInfoUser.find(StrXmlName);
	if ( it != m_mapCfmInfoUser.end() )
	{
		// 取消窗口的绑定
		CMPIChildFrame *pChildFrame = GetCfmFrame(StrXmlName);
		if ( NULL != pChildFrame )
		{
			BindCfm2ChildFrame(StrXmlName, pChildFrame, false);
		}

		::DeleteFile(it->second.m_StrFilePath);  // 要保证是在用户目录下的
		m_mapCfmInfoUser.erase(it);

		// 删除了之后可能会导致last...等不同步
	}
}

void CCfmManager::CurCfmSaveAs( const CString &StrSaveAsXmlName )
{
	ASSERT( IsInitialized() );

	// cfm不支持覆盖
	T_CfmFileInfo cfm;
	if ( QueryUserCfm(StrSaveAsXmlName, cfm) )
	{
		ASSERT( 0 );
		return;
	}
	
	// 将当前窗口另存为页面文件, 如果当前窗口有绑定的属性，则属性变更为另存为的页面属性
	CMainFrame *pMainFrame = (CMainFrame *)AfxGetMainWnd();
	ASSERT( pMainFrame != NULL && pMainFrame->IsKindOf(RUNTIME_CLASS(CMainFrame)) );
	if ( NULL != pMainFrame )
	{
		CMPIChildFrame *pChildFrame = (CMPIChildFrame *)pMainFrame->GetActiveFrame();
		if ( NULL != pChildFrame && pChildFrame->IsKindOf(RUNTIME_CLASS(CMPIChildFrame)) )
		{
			SaveCfm(StrSaveAsXmlName, pChildFrame);
			if ( QueryUserCfm(StrSaveAsXmlName, cfm) )
			{
				BindCfm2ChildFrame(StrSaveAsXmlName, pChildFrame);
			}
		}
	}
	ChangeOpenedCfm(StrSaveAsXmlName);
}

void CCfmManager::SaveCurrentCfm()
{
	CMainFrame *pMainFrame = (CMainFrame *)AfxGetMainWnd();
	ASSERT( pMainFrame != NULL && pMainFrame->IsKindOf(RUNTIME_CLASS(CMainFrame)) );
	if ( NULL != pMainFrame )
	{
		CMPIChildFrame *pChildFrame = (CMPIChildFrame *)pMainFrame->GetActiveFrame();
		if ( NULL != pChildFrame && pChildFrame->IsKindOf(RUNTIME_CLASS(CMPIChildFrame)) )
		{
			CString StrId = pChildFrame->GetIdString();
			T_CfmFileInfo cfm;
			if ( QueryUserCfm(StrId, cfm) )
			{
				SaveCfm(StrId, pChildFrame);	// 保存
			}
		}
	}
}

void CCfmManager::SaveCfm( const CString &StrXmlName, CMPIChildFrame *pSrcFrame )
{
#ifdef _DEBUG
	DWORD dwTime = timeGetTime();
#endif

	CMPIChildFrame *pChildFrame = pSrcFrame;
	if ( NULL == pChildFrame )
	{
		CMainFrame *pMainFrame = (CMainFrame *)AfxGetMainWnd();
		ASSERT( pMainFrame != NULL && pMainFrame->IsKindOf(RUNTIME_CLASS(CMainFrame)) );
		if ( NULL != pMainFrame )
		{
			CMPIChildFrame * pChild =(CMPIChildFrame * )pMainFrame->GetActiveFrame();
			if (NULL != pChild && pChild->IsKindOf(RUNTIME_CLASS(CMPIChildFrame)) )
			{
				pChildFrame = pChild;
			}	
		}
	}
	if ( NULL == pChildFrame )
	{
		TRACE(_T("无页面可以保存到: %s\n"), StrXmlName);
		return;
	}

	T_CfmFileInfo cfm;
	if ( IsUserDefaultCfm(StrXmlName) )
	{
		// 默认工作页面
		cfm = m_cfmUserDefault;
	}
	else if ( !QueryUserCfm(StrXmlName, cfm) )
	{
		// 不存在，只能尝试新建一个了
		AddNewUserCfm(StrXmlName);
		if ( !QueryUserCfm(StrXmlName, cfm) )
		{
			ASSERT( 0 );
			return;	// 无法获取wsp信息，不知道怎么处理了
		}
	}

	if ( StrXmlName.IsEmpty() )
	{
		ASSERT( 0 );	// 空文件页面怎么进来的
		return;
	}
	
	if ( cfm.m_bReadOnly )
	{
		TRACE(_T("忽略只读工作页面保存: %s\r\n"), cfm.m_StrFilePath);
		return;	// 特殊工作区以及只读工作区不能在单独保存, 可以通过另存来保存，因为另存wsp属性已经不是这个属性了
		// TODO 其它的保存规则
	}

	// 重组路径为private下
	if ( !cfm.m_bInUserPrivateDir )
	{
		CGGTongDoc* pDoc = (CGGTongDoc*)AfxGetDocument();
		cfm.m_bInUserPrivateDir = true;
		CString StrPath = CPathFactory::GetPrivateWorkspacePath(pDoc->m_pAbsCenterManager->GetUserName()) + cfm.m_StrFileName;
		RelativePathToAbsPath(StrPath, cfm.m_StrFilePath);
	}
	
	USES_CONVERSION;
	CString KStrXmlRootElementAttrDataChildFrame = L"ChildFrame";
	TiXmlDocument tiDoc;
	char szDataName[200];
	::WideCharToMultiByte(CP_UTF8, 0, KStrXmlRootElementAttrDataChildFrame, -1, szDataName, sizeof(szDataName), NULL, NULL);
	string strFileVersion;
	UnicodeToUtf8(cfm.m_StrFileVersion, strFileVersion);
	TiXmlElement *pRoot = ConstructGGTongAppXmlDocHeader(tiDoc, 
		szDataName,
		strFileVersion.c_str(),
		NULL,
		cfm.m_bReadOnly ? "1" : "0");
	CString StrEle = pChildFrame->ToXml(0, NULL);

// 	CStdioFile FileTe;
// 	if ( FileTe.Open(cfm.m_StrFilePath+_T(".test"), CFile::modeCreate|CFile::modeWrite|CFile::typeBinary) )
// 	{
// 		FileTe.WriteString(StrEle);
// 		FileTe.Close();
// 	}

	TiXmlDocument tiCfmDoc;
	string strContent;
	UnicodeToUtf8(StrEle, strContent);
	tiCfmDoc.Parse(strContent.c_str());
	pRoot->InsertEndChild(*tiCfmDoc.FirstChildElement());

	// 额外属性
	CString StrNum;
	// update mode
	StrNum.Format(_T("%d"), cfm.m_iUpdateMode);
	pRoot->SetAttribute(KStrEleAttriUpdateMode, _W2A(StrNum));
	// lock split
	pRoot->SetAttribute(KStrEleAttriLockSplit, cfm.m_bLockSplit ? "1" : "0");
	// secret
	pRoot->SetAttribute(KStrEleAttriSecret, cfm.m_bSecret ? "1" : "0");
	// auth
	pRoot->SetAttribute(KStrEleAttriNeedServerAuth, cfm.m_bNeedServerAuth ? "1" : "0");
	// system show
	StrNum.Format(_T("%ld"), cfm.m_iSystemShow);
	pRoot->SetAttribute(KStrEleAttriSystemShow, _W2A(StrNum));
	// from system
	pRoot->SetAttribute(KStrEleAttriFromSystem, cfm.m_bFromSystem ? "1" : "0");

	SaveTiXmlDoc(tiDoc, cfm.m_StrFilePath);

	// 测试version乱码
// 	T_CfmFileInfo cfmTest;
// 	ReadCfmInfoFromFile(cfm.m_StrFilePath, cfmTest);
// 	ASSERT( cfmTest.m_StrFileVersion == cfm.m_StrFileVersion );

	UpdateUserCfm(StrXmlName, cfm);	// 更新其属性

#ifdef _DEBUG
	TRACE(_T("关闭页面[%s]: %d ms\r\n"), StrXmlName.operator LPCTSTR(), timeGetTime()-dwTime);
#endif
}

CMPIChildFrame * CCfmManager::GetCurrentCfmFrame()
{
	CMainFrame *pMainFrame = (CMainFrame *)AfxGetMainWnd();
	if ( NULL != pMainFrame && pMainFrame->IsKindOf(RUNTIME_CLASS(CMainFrame)) )
	{
		CMPIChildFrame *pChildFrame = (CMPIChildFrame *)pMainFrame->MDIGetActive();
		if ( NULL != pChildFrame && !pChildFrame->GetIdString().IsEmpty() )
		{
			return pChildFrame;
		}
	}
	return NULL;
}

bool32 CCfmManager::IsCurrentCfm( const CString &StrXmlName )
{
	CMainFrame *pMainFrame = (CMainFrame *)AfxGetMainWnd();
	if ( NULL != pMainFrame && pMainFrame->IsKindOf(RUNTIME_CLASS(CMainFrame)) )
	{
		CMPIChildFrame *pChildFrame = (CMPIChildFrame *)pMainFrame->MDIGetActive();
		if ( NULL != pChildFrame && pChildFrame->GetIdString().CompareNoCase(StrXmlName) == 0 )
		{
			return true;
		}
	}
	return false;
}

CMPIChildFrame * CCfmManager::GetUserDefaultCfmFrame()
{
	CMainFrame *pMainFrame = (CMainFrame *)AfxGetMainWnd();
	CString StrDefault = GetUserDefaultCfm().m_StrXmlName;
	ASSERT( pMainFrame != NULL && pMainFrame->IsKindOf(RUNTIME_CLASS(CMainFrame)) );
	if ( NULL != pMainFrame && !StrDefault.IsEmpty() )
	{
		CWnd *pWnd = CWnd ::FromHandle(pMainFrame->m_hWndMDIClient)->GetWindow(GW_CHILD); 
		
		while ( NULL != pWnd )   
		{ 
			CWnd* pTmp		= pWnd;
			CWnd* pTmpNext	= pTmp->GetWindow(GW_HWNDNEXT);
			
			if ( pWnd->IsKindOf(RUNTIME_CLASS(CMPIChildFrame)) )
			{
				CMPIChildFrame *pChild = DYNAMIC_DOWNCAST(CMPIChildFrame, pWnd);
				if ( StrDefault.CompareNoCase(pChild->GetIdString()) == 0 )
				{
					return pChild;
				}
			}
			pWnd = pTmpNext;
		}
	}
	return NULL;
}

CMPIChildFrame * CCfmManager::GetCfmFrame( const CString &StrXmlName )
{
	CMainFrame *pMainFrame = (CMainFrame *)AfxGetMainWnd();
	ASSERT( pMainFrame != NULL && pMainFrame->IsKindOf(RUNTIME_CLASS(CMainFrame)) );
	if ( NULL != pMainFrame )
	{
		CWnd *pWnd = CWnd ::FromHandle(pMainFrame->m_hWndMDIClient)->GetWindow(GW_CHILD); 
		
		while ( NULL != pWnd )   
		{ 
			CWnd* pTmp		= pWnd;
			CWnd* pTmpNext	= pTmp->GetWindow(GW_HWNDNEXT);
			
			if ( pWnd->IsKindOf(RUNTIME_CLASS(CMPIChildFrame)) )
			{
				CMPIChildFrame *pChild = DYNAMIC_DOWNCAST(CMPIChildFrame, pWnd);
				if ( StrXmlName.CompareNoCase(pChild->GetIdString()) == 0 )
				{
					return pChild;
				}
			}
			pWnd = pTmpNext;
		}
	}
	return NULL;
}

void CCfmManager::ChangeIoViewStatus(CMPIChildFrame *pChild)
{
	CMainFrame *pMainFrame = (CMainFrame *)AfxGetMainWnd();
	for (int32 i=0; i<pMainFrame->m_IoViewsPtr.GetSize(); i++)
	{
		CIoViewBase * pIoView = pMainFrame->m_IoViewsPtr[i];

		pIoView->m_bShowNow = false;
		pIoView->SetKillerTimer(false);

		if(pChild == pIoView->GetParentFrame())
		{
			if(pIoView->GetIoViewManager())
			{
				if(!pIoView->GetIoViewManager()->GetActiveIoView()->m_bShowNow)
				{
					pIoView->GetIoViewManager()->GetActiveIoView()->m_bShowNow = true;
					pIoView->GetIoViewManager()->GetActiveIoView()->SetHasIgnoreLastViewDataReqFlag(true);
					pIoView->GetIoViewManager()->GetActiveIoView()->SetKillerTimer(true);
				}
			}
			else
			{
				pIoView->m_bShowNow = true;
				if(pIoView->IsWindowVisible())
				{
					pIoView->SetHasIgnoreLastViewDataReqFlag(true);
					pIoView->SetKillerTimer(true);
				}
			}
		}
	}
}

int32 CCfmManager::CloseCfmFrame( const CString &StrXmlName, bool32 bSaveCfm/* = false*/ )
{
	int32 iRet = 0;
	CMainFrame *pMainFrame = (CMainFrame *)AfxGetMainWnd();
	ASSERT( pMainFrame != NULL && pMainFrame->IsKindOf(RUNTIME_CLASS(CMainFrame)) );
	if ( NULL != pMainFrame )
	{
		CWnd *pWnd = CWnd ::FromHandle(pMainFrame->m_hWndMDIClient)->GetWindow(GW_CHILD); 
		
		while ( NULL != pWnd )   
		{ 
			CWnd* pTmp		= pWnd;
			CWnd* pTmpNext	= pTmp->GetWindow(GW_HWNDNEXT);
			
			if ( pWnd->IsKindOf(RUNTIME_CLASS(CMPIChildFrame)) )
			{
				CMPIChildFrame *pChild = DYNAMIC_DOWNCAST(CMPIChildFrame, pWnd);
				if ( StrXmlName.CompareNoCase(pChild->GetIdString()) == 0 && !IsReserveCfm(StrXmlName))
				{
					// 这里关闭的窗口并不自动保存
					if ( bSaveCfm )
					{
						SaveCfm(StrXmlName, pChild);
					}
					pChild->PostMessage(WM_CLOSE, 0, 0);
					iRet++;
				}
			}
			pWnd = pTmpNext;
		}
	}

	return iRet;
}

bool32 CCfmManager::LoadCurCfm()
{
	// 加载当前窗口绑定的页面文件, 如果有的话
	CMainFrame *pMainFrame = (CMainFrame *)AfxGetMainWnd();
	ASSERT( pMainFrame != NULL && pMainFrame->IsKindOf(RUNTIME_CLASS(CMainFrame)) );
	if ( NULL != pMainFrame )
	{
		CMPIChildFrame *pChildFrame = (CMPIChildFrame *)pMainFrame->GetActiveFrame();
		if ( NULL != pChildFrame && pChildFrame->IsKindOf(RUNTIME_CLASS(CMPIChildFrame)) )
		{
			CString StrId = pChildFrame->GetIdString();
			T_CfmFileInfo cfm;
			if ( QueryUserCfm(StrId, cfm) )
			{
				return LoadCfm(StrId, true) != NULL;	// 此后该childframe被关闭了
			}
		}
	}
	return false;
}

CMPIChildFrame * CCfmManager::LoadCfm( const CString &StrXmlName, bool32 bCloseExistence /*= false*/, bool32 bHideNewCfm/*=false*/, CString StrCfmUrl, bool bDelete )
{
	// 理解为关闭已经打开了该页面文件的窗口，然后新建窗口并绑定指定页面文件打开

	ASSERT( IsInitialized() );
	T_CfmFileInfo cfm;
	
	if ( QueryUserCfm(StrXmlName, cfm) )
	{
		if ( cfm.m_bNeedServerAuth )		// 检查权限
		{
			if ( !CPluginFuncRight::Instance().IsUserHasRight(StrXmlName, true) )
				return NULL;
		}

		ASSERT( !StrXmlName.IsEmpty() );
		
		CMainFrame *pMainFrame = (CMainFrame *)AfxGetMainWnd();
		ASSERT( pMainFrame != NULL && pMainFrame->IsKindOf(RUNTIME_CLASS(CMainFrame)) );
		if ( NULL != pMainFrame )
		{
			// TODO mainframe应当打开这个工作页面
			//
			if ( _taccess(cfm.m_StrFilePath, 0) != 0 )
			{
				// 不存在这个文件，只有替换这次打开操作了
				ASSERT( 0 );
				T_CfmFileInfo cfmSys;
				if ( cfm.m_bSystemDirHas
					&& QuerySysCfm(StrXmlName, cfmSys) )
				{
					cfm.m_StrFilePath = cfmSys.m_StrFilePath;
				}
			}

            if(!bDelete)
            {
                AddOpenedCfm(StrXmlName, StrCfmUrl);
            }

			// 查找已经打开了的该页面, 关闭它
			CMPIChildFrame *pChildFrame = NULL;
			if ( !bCloseExistence && NULL != (pChildFrame=GetCfmFrame(StrXmlName)) )
			{
				//if ( cfm.m_bNeedReload )
				//{
				//	CloseCfmFrame(StrXmlName);
				//	pChildFrame = (CMPIChildFrame *)pMainFrame->OpenChildFrame(cfm.m_StrXmlName, bHideNewCfm);
				//	ASSERT( NULL != pChildFrame );
	
				//	if ( NULL != pChildFrame )
				//	{
				//		BindCfm2ChildFrame(StrXmlName, pChildFrame);
				//		pMainFrame->MDIActivate(pChildFrame);
				//	}
				//}
				//else
				{
					ChangeIoViewStatus(pChildFrame);

					::SendMessage(pMainFrame->m_hWndMDIClient, WM_SETREDRAW, FALSE, 0L);
					pMainFrame->MDIActivate(pChildFrame);
					::SendMessage(pMainFrame->m_hWndMDIClient, WM_SETREDRAW, TRUE, 0L);
					::RedrawWindow(pMainFrame->m_hWndMDIClient, NULL, NULL, 
						RDW_UPDATENOW | RDW_ALLCHILDREN | RDW_INVALIDATE);//强制重绘，包括子窗口
				}
				

				return pChildFrame;	// 不关闭则直接返回 - 相当于打开了
			}
			else if ( bCloseExistence )
			{
				CloseCfmFrame(StrXmlName);
			}

			// 不更新cfm属性			
			pChildFrame = (CMPIChildFrame *)pMainFrame->OpenChildFrame(cfm.m_StrXmlName, bHideNewCfm);
			ASSERT( NULL != pChildFrame );
		
			if ( NULL != pChildFrame )
			{
				BindCfm2ChildFrame(StrXmlName, pChildFrame);
				pMainFrame->MDIActivate(pChildFrame);
				// 加载完版面后添加到标签
				/////////////////////////////////////////////////////
				//===================CODE CLEAN======================
				//////////////////////////////////////////////////////
				if(!StrXmlName.IsEmpty())
				{
					//CMainFrame *pMainFrm = DYNAMIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
					//if (NULL != pMainFrm)
					//{
					//	pMainFrm->m_leftToolBar.ChangeLeftBarStatus(StrXmlName);
					//}
					CNewCaptionTBWnd::AddPage(StrXmlName);
				}
				
				// 关闭其它的系统页面
				//CloseOtherSameClassCfm(StrXmlName);
				return pChildFrame;
			}
		}
	}
	return NULL;
}

void CCfmManager::UpdateUserCfm( const CString &StrXmlName, const T_CfmFileInfo &cfmInfo )
{
	if ( cfmInfo.m_bReadOnly  )
	{
		return;		// 带有此标志的用户工作区不允许更新
	}
	T_CfmFileInfo cfmTmp;
	if ( QueryUserCfm(StrXmlName, cfmTmp) )
	{
		if ( cfmTmp.m_bReadOnly )
		{
			return;	// 此不能更新于用户中， 无论是系统的还是用户的
		}
	}
	m_mapCfmInfoUser[ StrXmlName ] = cfmInfo;
}

CCfmManager & CCfmManager::Instance()
{
	static CCfmManager sWspManager;
	return sWspManager;
}

bool32 CCfmManager::SetUserDefaultCfm( const CString &StrXmlName )
{
	// 影响是从程序下次启动开始，不影响此次
	ASSERT( IsInitialized() );
	if ( IsInitialized() )
	{
		SetUserLastSelCfm(StrXmlName, m_StrUserName);
		return true;
	}
	return false;
}

bool32 CCfmManager::CheckCfmExist( const CString &StrXmlName, const CfmInfoMap &infoMap, OUT T_CfmFileInfo *pCfmInfo )const
{
	CfmInfoMap::const_iterator it = infoMap.find(StrXmlName);
	if ( it != infoMap.end() )
	{
		if ( NULL != pCfmInfo )
		{
			*pCfmInfo = it->second;
		}
		return true;	
	}
	
	return false;
}

DWORD CCfmManager::GetCfmUserRights( const CString &StrXmlName ) const
{
	T_CfmFileInfo cfm;
	if ( QueryUserCfm(StrXmlName, cfm) )
	{
		return GetCfmUserRights(cfm);
	}
	return 0;
}

DWORD CCfmManager::GetCfmUserRights( const T_CfmFileInfo &info ) const
{
	DWORD dwRights = 0;
	
	// 看是否具有文件名, 不检查文件存在否
	if ( !info.m_StrFilePath.IsEmpty() )
	{
		dwRights |= ECUR_Open;
	}

	if ( !info.m_bSecret )
	{
		dwRights |= ECUR_ShowInList;
	}

	if ( info.m_bInUserPrivateDir && !info.m_bSystemDirHas )
	{
		if ( !IsUserDefaultCfm(info.m_StrXmlName) )	// 默认页面不能删除
		{
			dwRights |= ECUR_Delete;
		}
	}

	if ( info.m_bInUserPrivateDir && info.m_bSystemDirHas )
	{
		dwRights |= ECUR_Restore;
	}

	if ( !info.m_bReadOnly )
	{
		dwRights |= ECUR_Write;
	}

	return dwRights;
}

void CCfmManager::BindCfm2ChildFrame( const CString &StrXmlName, CMPIChildFrame *pChildFrame, bool32 bBind /*= true*/ )
{
	if ( NULL != pChildFrame )
	{
		bBind = bBind && !StrXmlName.IsEmpty();
		if ( bBind )
		{
			pChildFrame->SetIdString(StrXmlName);
			pChildFrame->SetChildFrameTitle(StrXmlName, true);
		}
		else
		{
			pChildFrame->SetIdString(_T(""));
			pChildFrame->SetFixTitleFlag(false);
		}
	}
}

const std::pair<DWORD, CString> sESDC2CfmName[] = {
	std::pair<DWORD, CString>(CCfmManager::ESDC_Report, _T("报价列表")),
		std::pair<DWORD, CString>(CCfmManager::ESDC_KLine, _T("K线分析")),
		std::pair<DWORD, CString>(CCfmManager::ESDC_Trend, _T("分时走势")),
		std::pair<DWORD, CString>(CCfmManager::ESDC_News, _T("新闻资讯")),
		std::pair<DWORD, CString>(CCfmManager::ESDC_PhaseSort, _T("阶段排行")),
		std::pair<DWORD, CString>(CCfmManager::ESDC_KlineArbitrage, _T("套利K线")),
		std::pair<DWORD, CString>(CCfmManager::ESDC_TrendArbitrage, _T("套利分时")),
		std::pair<DWORD, CString>(CCfmManager::ESDC_ReportArbitrage, _T("套利报价表")),
		std::pair<DWORD, CString>(CCfmManager::ESDC_HomePage, _T("首页")),
};
const DWORD sESDC2CfmCount = sizeof(sESDC2CfmName)/sizeof(sESDC2CfmName[0]);

void CCfmManager::GetSystemDefaultCfmNames( OUT CStringArray &aNames )
{
	aNames.RemoveAll();
	
	for (int i=0; i < ESDC_Count ; i++)
	{
		ASSERT( sESDC2CfmName[i].first == i );
		aNames.Add(sESDC2CfmName[i].second);
	}
}
/*	
	error 568: (Warning -- non-negative quantity is never less than zero)
*/
//lint --e{568}
bool32 CCfmManager::GetSystemDefaultCfmName( IN E_SystemDefaultCfm esdc, OUT CString &StrName )
{
	if ( esdc >= 0 && esdc < ESDC_Count )
	{
		StrName = sESDC2CfmName[esdc].second;
		return true;
	}
	return false;
}

CCfmManager::E_SystemDefaultCfm CCfmManager::GetESDC( const CString &StrName )
{
	for (int i=0; i < ESDC_Count ; i++)
	{
		ASSERT( sESDC2CfmName[i].first == i );
		if ( sESDC2CfmName[i].second.CompareNoCase(StrName) == 0 )
		{
			return (E_SystemDefaultCfm)sESDC2CfmName[i].first;
		}
	}
	return ESDC_Count;
}

int32 CCfmManager::CloseOtherSameClassCfm( const CString &StrXmlNameReserve )
{
	// 关闭指定名字以外的任何系统cfm(默认 图表除外)
	int32 iClose = 0;
	CMainFrame *pMainFrame = (CMainFrame *)AfxGetMainWnd();
	ASSERT( pMainFrame != NULL && pMainFrame->IsKindOf(RUNTIME_CLASS(CMainFrame)) );

	
	// 如果是本来就需要保留的页面就可以不关闭其它页面了，因为不会引起页面新增
	// 需要保留的几个
	CStringArray aReserveName;
	GetReserveCfmNames(aReserveName);
	if ( IsReserveCfm(StrXmlNameReserve) )
	{// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		return 0;
	}

	if ( !StrXmlNameReserve.IsEmpty() )
	{
		aReserveName.Add(StrXmlNameReserve);	// 将自己排除在外
	}

	CStringArray aStrCaption;
	CNewCaptionTBWnd::GetCaptionArray(aStrCaption);
	for (int i = 0; i < aStrCaption.GetSize(); ++i)
	{	
		//--- wangyongxue 不能无休止添加
		int iCaptionSize = aStrCaption.GetSize();
		
		if (iCaptionSize > 10)
		{
			break;
		}
		
		aReserveName.Add( aStrCaption.GetAt(i));
	}
	

	T_CfmFileInfo cfmReserve;
	CfmInfoMap mapDelCfms;
	if ( QueryUserCfm(StrXmlNameReserve, cfmReserve) )
	{
		if ( cfmReserve.m_bSystemDirHas )	// 系统
		{
			QueryUserSystemCfm(mapDelCfms);
		}
		else
		{
			QueryUserPrivateCfm(mapDelCfms); // 用户
		}
	}
	else
	{
		// 关闭所有的
		QueryUserAllCfm(mapDelCfms);
	}
	
	for ( int32 i=0; i < aReserveName.GetSize() ; i++ )
	{
		mapDelCfms.erase( aReserveName[i] );	// 移出这些保留的名字
	}

	if ( NULL != pMainFrame )
	{
		CWnd *pWnd = CWnd ::FromHandle(pMainFrame->m_hWndMDIClient)->GetWindow(GW_CHILD); 
		
		while ( NULL != pWnd )   
		{ 
			CWnd* pTmp		= pWnd;
			CWnd* pTmpNext	= pTmp->GetWindow(GW_HWNDNEXT);
			
			if ( pWnd->IsKindOf(RUNTIME_CLASS(CMPIChildFrame)) )
			{
				CMPIChildFrame *pChild = DYNAMIC_DOWNCAST(CMPIChildFrame, pWnd);
				CString StrId = pChild->GetIdString();
				if ( !StrId.IsEmpty() && mapDelCfms.count(StrId) > 0 )
				{
					SaveCfm(StrId, pChild);	// 自动保存被关闭的
					pChild->PostMessage(WM_CLOSE, 0, 0);
					iClose++;
				}
			}
			pWnd = pTmpNext;
		}
	}
	return iClose;
}

bool32 CCfmManager::IsReserveCfm( const CString &StrXmlName )
{
	// 需要保留的几个
	CStringArray aReserveName;
	GetReserveCfmNames(aReserveName);
	for ( int32 i=0; i < aReserveName.GetSize() ; i++ )
	{
		if ( aReserveName[i] == StrXmlName )
		{
			return true;
		}
	}
	return false;
}

void CCfmManager::GetReserveCfmNames( OUT CStringArray &aReserveName )
{
	// 需要保留的几个
	CString StrTrend, StrKline, StrPhaseSort, StrReport;
	GetSystemDefaultCfmName(CCfmManager::ESDC_Trend, StrTrend);
	GetSystemDefaultCfmName(CCfmManager::ESDC_KLine, StrKline);
	GetSystemDefaultCfmName(CCfmManager::ESDC_PhaseSort, StrPhaseSort);
	GetSystemDefaultCfmName(CCfmManager::ESDC_Report, StrReport);
	aReserveName.Add(StrKline);
	aReserveName.Add(StrTrend);
	aReserveName.Add(StrPhaseSort);		// 阶段排行页面不自动关闭
	aReserveName.Add(StrReport);
	aReserveName.Add(_T("闪电图"));

	//	不保留首页，登录情况下需要重新加载版面数据
	CString StrDefCfm = GetUserDefaultCfm().m_StrXmlName;
	if ( !StrDefCfm.IsEmpty() && (StrDefCfm!=StrReport))
	{
		//aReserveName.Add( GetUserDefaultCfm().m_StrXmlName );
	}
}

bool32 CCfmManager::ModifyUserCfmAttri( const T_CfmFileInfo &cfmInfo )
{
	CfmInfoMap::iterator it = m_mapCfmInfoUser.find(cfmInfo.m_StrXmlName);
	if ( it != m_mapCfmInfoUser.end() )
	{
		it->second.m_StrFileVersion = cfmInfo.m_StrFileVersion;
		it->second.m_iUpdateMode	= cfmInfo.m_iUpdateMode;
		it->second.m_bLockSplit		= cfmInfo.m_bLockSplit;
		it->second.m_bSecret		= cfmInfo.m_bSecret;
		it->second.m_bReadOnly		= cfmInfo.m_bReadOnly;
		return true;
	}
	return false;
}

bool32 CCfmManager::QuerySysCfm( const CString &StrCfmName, OUT T_CfmFileInfo &cfmInfo )
{
	return CheckCfmExist(StrCfmName, m_mapCfmInfoSystem, &cfmInfo);
}

CString CCfmManager::GetProperFileExt( bool32 bUserCfm )
{
	if ( bUserCfm )
	{
		return _T(".cfu");	// 用户的工作页面文件后缀名
	}
	else
	{
		return _T(".cfm"); // 系统的工作页面文件的后缀名
	}
}

bool32 CCfmManager::IsCfmExist( const CString &StrXmlName )
{
	T_CfmFileInfo info;
	return QueryUserCfm(StrXmlName, info);
}

void CCfmManager::GetOpenedCfm(CfmInfoArray &aStrCfm)
{
	aStrCfm.Copy(m_aStrOpenedCfm);
}

void CCfmManager::SetOpenedCfm()
{
	int32 iSize = m_aStrOpenedCfm.GetSize();
	m_aStrOpenedCfm.RemoveAt(iSize-1);
}

void CCfmManager::ReMoveAllOpendCfm()
{
	m_aStrOpenedCfm.RemoveAll();
}

void CCfmManager::ChangeOpenedCfm(CString strCfmName, CString strCfmUrl)
{
	int32 iCfmSize = m_aStrOpenedCfm.GetSize();
	for (int32 i=0; i<iCfmSize; i++)
	{
		if (m_aStrOpenedCfm[i].StrCfmName == strCfmName)
		{
			m_aStrOpenedCfm.RemoveAt(i);
			break;
		}
	}
	
	if(!strCfmName.IsEmpty())
	{
		CNewCaptionTBWnd::AddPage(strCfmName);
	}
    
	T_CfmInfo stCfmInfo;
    stCfmInfo.StrCfmName = strCfmName;
    stCfmInfo.StrCfmUrl = strCfmUrl;
	m_aStrOpenedCfm.Add(stCfmInfo);
}

void CCfmManager::AddOpenedCfm( CString strCfmName, CString strCfmUrl /*= L""*/ )
{
    T_CfmInfo stCfmInfo;
    stCfmInfo.StrCfmName = strCfmName;
    stCfmInfo.StrCfmUrl = strCfmUrl;
    m_aStrOpenedCfm.Add(stCfmInfo);
}

//////////////////////////////////////////////////////////////////////////
T_CfmFileInfo::T_CfmFileInfo()
{
	m_bReadOnly = false;
	m_bSystemDirHas = false;
	m_bInUserPrivateDir = true;
	m_iUpdateMode = 0;
	m_bSecret = false;
	m_bLockSplit = false;
	m_bNeedServerAuth = false;

	m_iSystemShow = 0;
	m_bFromSystem = false;
	m_bNeedReload = false;
}
