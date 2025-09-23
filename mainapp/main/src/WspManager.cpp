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
// ���ÿ���û��򿪵Ĺ�������¼ xml
static const char  *KStrLastUserSelCfmFileName = "UserDefautCfm.xml";
static const char  *KStrLastUserSelCfmKey = "record";
static const char  *KStrLastUserSelCfmAttriUserName = "user";
static const char  *KStrLastUserSelCfmAttriCfmXmlName = "cfm";

static const CString  KStrLastUserSelDefaultUserName = _T("");	//  Ĭ��������
static CString  KStrLastUserSelDefaultWspXmlName = _T("��ҳ");	//  Ĭ�������� - �ᱻ����Ϊ����ѡ������

// xml
static const char * KStrElementAttriFileShowName	= "FileShowName";

static const char *KStrEleAttriSecret = "secret";	
static const char *KStrEleAttriUpdateMode  = "updateMode";
static const char *KStrEleAttriLockSplit = "lockSplit";
static const char *KStrEleAttriNeedServerAuth = "needAuth";
static const char *KStrEleAttriSystemShow = "system";
static const char *KStrEleAttriFromSystem = "fromSystem";	// ��ϵͳ�ļ����Ƶ�
static const char *KStrEleAttriNeedReload = "needReload";	// �ٴδ�cfm�Ƿ���Ҫ���¼���


//////////////////////////////////////////////////////////////////////////
CCfmManager::CCfmManager()
{
	// ǿ�ƽ� KStrLastUserSelDefaultWspXmlName ͬ��
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

	// ȷ��˽���ļ��д���
	CString StrPrivateWspPath = CPathFactory::GetPrivateWorkspacePath(StrUserName);
	_tcheck_if_mkdir(StrPrivateWspPath.GetBuffer(MAX_PATH));
	StrPrivateWspPath.ReleaseBuffer();

	// ���Ҹ��û������й�������Ϣ
	RefreshCfm();

	//  �Ƿ�����ʱ�ļ������й������Զ����浽��ʱĿ¼����������ʱ�������ʱĿ¼
	//	�����ʾ�Ƿ񱣴湤������������棬�����д��ڵ���ʱ�ļ�ת�浽˽��Ŀ¼
	//	�����Զ��������еĸ���

	// ��ʼ��Last wsp
	if ( GetUserLastSelCfm(m_cfmUserDefault, StrUserName) )	// �ڲ��趨Ĭ�Ϲ���ҳ��
	{
		SetUserLastSelCfm(m_cfmUserDefault.m_StrXmlName, StrUserName);	// ����һ��
	}
	else
	{
		AddNewUserCfm(KStrLastUserSelDefaultWspXmlName);	// ����һ��Ĭ�ϵ�cfm
		QueryUserCfm(KStrLastUserSelDefaultWspXmlName, m_cfmUserDefault);	// ��ʼ��Ĭ��
		SetUserLastSelCfm(m_cfmUserDefault.m_StrXmlName, StrUserName);	// ����һ��
	}
}

void CCfmManager::RefreshCfm()
{
	ASSERT( !m_StrUserName.IsEmpty() );
	
	m_mapCfmInfoSystem.clear();
	m_mapCfmInfoUser.clear();
	
	// ���Ҹ��û������й�������Ϣ
	CfmInfoMap cfmInfos;
	// ��ʱ��ʼ����wsp����SystenHas��׼ȷ���ȳ�ʼ��system
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
	// user��Ϣ���ܲ�׼ȷ �ٳ�ʼ��user
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
	
	
	// TODO ����ҳ��
	// ������˽�п��ܻ��ѹ죬���ܻ���ڴӹ��й����ĵ������ڹ���û���ˣ��Ӷ����ڱ�ɨ��Ϊ˽�е�
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
	// �����������ڸ��û��Ĺ��������û�Ŀ¼�µĹ���������ͬxml����ϵͳ������
	cfmInfos.clear();
	cfmInfoSys.clear();
	cfmInfoUser.clear();

	if ( NULL == pszUserName )
	{
		pszUserName = m_StrUserName;	// ʹ�ñ���ĵ�ǰ�û�
	}

	// �п����ǿյ��û�~�� ���û�������ϵͳ��

	// �ȼ���ϵͳ��, ϵͳ����ʵʱɨ�裬 ����ʹ������ɨ���
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
				TRACE(_T("ϵͳcfm���ع����з������Ƴ�ͻ, ������Ϊ: %s\r\n"), cfmInfo.m_StrXmlName);
			}
			cfmInfos[ cfmInfo.m_StrXmlName ] = cfmInfo;
			cfmInfoSys[ cfmInfo.m_StrXmlName ] = cfmInfo;
		}
	}

	// �ڿ��û����ܲ��ܼ���
	if ( pszUserName != NULL && pszUserName[0] != _T('\0') )
	{
		// �ȳ���.cfmϵͳ��ʽ�Լ���ǰ�����ĸ�ʽ���ļ�, ֻ�����ָ�ʽ�ĲŻ���Ϊ��ϵͳ�ļ�����
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
				// ��ʱ��systemDirHas��inPrivate���ܻ��ж�ʧ�󣬲�������ֻ�ڵ�¼ǰ�Ż��������û������
				// �����û��Ŀ��Ը���ϵͳ�ģ���������ֻ��Ҫ���û��Լ��ļ��Ϳ�����
				int iCanAdd = CheckCfmCanAdd(cfmInfo, cfmInfoUser, true);
				ASSERT( iCanAdd >= 0 );
				if ( iCanAdd > 0 )
				{
					TRACE(_T("�û�Ŀ¼��ϵͳ[%s]wsp���ع����з������Ƴ�ͻ, ������Ϊ: %s\r\n"), pszUserName, cfmInfo.m_StrXmlName);
				}
				// ��ʱuser��Ϣsystem has���ܲ�׼ȷ, ������
				if ( !cfmInfo.m_bSystemDirHas && cfmInfoSys.count(cfmInfo.m_StrXmlName) > 0 )
				{
					ASSERT( cfmInfo.m_bInUserPrivateDir );
					cfmInfo.m_bSystemDirHas = true;
				}
				cfmInfos[ cfmInfo.m_StrXmlName ] = cfmInfo;	// ���ǻ�������
				cfmInfoUser[ cfmInfo.m_StrXmlName ] = cfmInfo;
			}
		}

		// �ٳ���.cfu�û������ļ�
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
				// ������ֵ��Ǵ��û�����ҳ�棬����ϵͳ��ͬ��Ҳ���ᴦ���ϵͳ��
				// ASSERT( !cfmInfo.m_bSystemDirHas );
				cfmInfo.m_bSystemDirHas = false;
				int iCanAdd = CheckCfmCanAdd(cfmInfo, cfmInfos, true);	// ���������й���ҳ�����Ƴ�ͻ
				ASSERT( iCanAdd >= 0 );
				if ( iCanAdd > 0 )
				{
					TRACE(_T("�û�[%s]wsp���ع����з������Ƴ�ͻ, ������Ϊ: %s\r\n"), pszUserName, cfmInfo.m_StrXmlName);
				}

				cfmInfos[ cfmInfo.m_StrXmlName ] = cfmInfo;	// ���ǻ�������
				cfmInfoUser[ cfmInfo.m_StrXmlName ] = cfmInfo;
			}
		}
	}
}

bool32 CCfmManager::ReadCfmInfoFromFile( const CString &StrFilePathOrg, OUT T_CfmFileInfo &cfmInfo, bool32 bSureSysteFile/* = false*/ ) const
{
	// �õ��������ļ���������XMLData���� version xml_name readonly
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

	// ���ж��ǲ���ͬһ���ļ����ڿ��Ƿ��Ѿ���ʼ��������Ѿ���ʼ�������û���ϵͳ�б�����һ���Ƿ���ͬxml����ϵͳwsp
	//    ��Ϊϵͳ�б�����ʲô�û�����ͬ�ģ����Բ��������û���
	// �绹��ȷ���Ͳ���Ҫ�ж��ˣ� �Ѿ����ܿ����жϣ�����Ҳ����Ҫ�ж�
	if ( !bSureSysteFile )
	{
		CFileFind fileFind;
		CString StrSysWspPath = CPathFactory::GetPublicWorkspacePath();
		StrSysWspPath.Replace(_T('\\'), _T('/'));
		StrSysWspPath += cfmInfo.m_StrFileName;
		if (fileFind.FindFile(StrSysWspPath))	// ϵͳĿ¼���Ƿ��иù������ļ�����
		{
			fileFind.FindNextFile();
			StrSysWspPath  = fileFind.GetFilePath();	// ��������ļ�ȫ��

			cfmInfo.m_bSystemDirHas = true;		// ϵͳ�ļ�����
			cfmInfo.m_bFromSystem = true;

			if ( fileFind.FindFile(StrFilePath) ) // ����ԭ�����ļ�
			{
				fileFind.FindNextFile();
				bool32 bSameFile = StrSysWspPath.CompareNoCase(fileFind.GetFilePath()) == 0;	// �����Ϊͬһ���ļ�
				
				cfmInfo.m_bInUserPrivateDir = !bSameFile;	// Ϊϵͳ�������ļ��������û�˽��Ŀ¼��
				
			}
		}
	}
	else
	{
		cfmInfo.m_bSystemDirHas = true;
		cfmInfo.m_bInUserPrivateDir = false;	// ��ȷ���ˣ��Ͳ��Ƚ���
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
	
	// ����
	if ( !cfmInfo.m_bSystemDirHas && cfmInfo.m_bInUserPrivateDir )
	{
		// ��˽��Ŀ¼���wsp�ļ��ٴ��û���system wsp����ȶ�һ��xml���ƣ����Ƿ�����ͬxml���Ƶģ����У���Ϊ��ģ��
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

	// ��������ʾ systemshow
	const char *pSystemShow = pRootElement->Attribute(KStrEleAttriSystemShow);
	if ( NULL != pSystemShow )
	{
		cfmInfo.m_iSystemShow = atol(pSystemShow);
	}

	// �Ƿ��ϵͳcfm����, �������ֻҪ��������ʱ�������������ã���������������ʱ����
	const char *pFromSystem = pRootElement->Attribute(KStrEleAttriFromSystem);
	if ( NULL != pFromSystem && !cfmInfo.m_bFromSystem )
	{
		cfmInfo.m_bFromSystem = atol(pFromSystem) != 0;
	}

	// �ٴδ�cfm�Ƿ���Ҫ���¼���
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

	// ���û��� ��ϵͳ
	if ( !CheckCfmExist(StrXmlName, m_mapCfmInfoUser, &cfmInfo) )
	{
		return CheckCfmExist(StrXmlName, m_mapCfmInfoSystem, &cfmInfo);
	}
	return true;
}

void CCfmManager::QueryUserSystemCfm( OUT CfmInfoMap &cfmInfos ) const
{
	ASSERT( !m_StrUserName.IsEmpty() );

	// ϵͳ�в��ң�����û����У���ʹ���û���ȡ��
	cfmInfos.clear();
	for (CfmInfoMap::const_iterator it = m_mapCfmInfoSystem.begin() ; it != m_mapCfmInfoSystem.end() ; it++)
	{
		CfmInfoMap::const_iterator itUser = m_mapCfmInfoUser.find(it->first);
		if ( itUser != m_mapCfmInfoUser.end() )
		{
			cfmInfos.insert(*itUser);// �û��У������
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

	// ���������û���
	cfmInfos.clear();
	for (CfmInfoMap::const_iterator it = m_mapCfmInfoUser.begin() ; it != m_mapCfmInfoUser.end() ; it++)
	{
		CfmInfoMap::const_iterator itSys = m_mapCfmInfoSystem.find(it->first);
		if ( itSys != m_mapCfmInfoSystem.end() || it->second.m_bSystemDirHas )
		{
			// ϵͳ����У�����Ϊ��private��
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
	
	// ���������û���
	cfmInfos.clear();

	cfmInfos = m_mapCfmInfoSystem;	// ��ϵͳ

	for (CfmInfoMap::const_iterator it = m_mapCfmInfoUser.begin() ; it != m_mapCfmInfoUser.end() ; it++)
	{
		cfmInfos[it->first] = it->second;	// �����򸲸�
	}
}


void CCfmManager::LoadPublicLastCfmList()
{
	// �Ƿ��ṩĬ�������趨��
	// �ȴӹ���Ŀ¼����Ĭ������
	TiXmlDocument tiDoc;
	CString StrDir = CPathFactory::GetPublicConfigPath();	
	StrDir.Replace(_T('\\'), _T('/'));
	StrDir += KStrLastUserSelCfmFileName;	// Ϊascii��ֱ��+

	m_mapUserLastSelCfm.clear();

	// �趨���ʼ��Ĭ���ʺŶ�Ӧ������
	m_mapUserLastSelCfm[ KStrLastUserSelDefaultUserName ] = KStrLastUserSelDefaultWspXmlName;

	// ����Ŀ¼���س�ʼ��ȫ�ֵ�Ĭ��
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
		StrDir += KStrLastUserSelCfmFileName;	// Ϊascii��ֱ��+

		CString StrSysDefaultCfmName = m_mapUserLastSelCfm[KStrLastUserSelDefaultUserName];	// �����û�ȱʡ��cfm

		for ( UserLastCfmMap::const_iterator it = m_mapUserLastSelCfm.begin() ; it != m_mapUserLastSelCfm.end() ; it++ )
		{
			// ��ȫ���û�ȱʡ��ͬ�Ķ�������, ȫ�����Ʋ�����
			if ( it->second.CompareNoCase(StrSysDefaultCfmName) == 0
				|| it->first.CompareNoCase(KStrLastUserSelDefaultUserName) == 0 )
			{
				// ȫ����
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
	// ��ȡ���û����е�wsp(��Ҫ���⹤����)
	CfmInfoMap cfmInfos, cfmSyss, cfmUsers;
	ScanUserAllCfm(cfmInfos, cfmSyss, cfmUsers, StrUserName);	// �������⹤����

	// ȷ�����򿪹������ļ�
	//	1. ��ȡ�ļ����ö�Ӧ���ƣ�����޷��ҵ���Ӧwsp������Ĭ��������(���������Ƿ��ж�Ӧ������)
	//  2. ���д��ʱ���������й������ļ�, �ҵ����д����ļ�
	//  3. �޺��ʵķ���ʧ��, mainframe��Ҫ�����û���κ�wsp�ļ�ʱ�����
	UserLastCfmMap::const_iterator itUserSelCfm = m_mapUserLastSelCfm.find(StrUserName);
	if ( itUserSelCfm != m_mapUserLastSelCfm.end() )
	{
		CfmInfoMap::const_iterator it = cfmInfos.find(itUserSelCfm->second);
		if ( it != cfmInfos.end() )
		{
			cfmInfo = it->second;
			return true;	// �ҵ��˺��ʵ�wsp�ļ�(���������¶�Ӧ���ǵ�)
		}
		// �п����ļ���ɾ����
		TRACE(_T("�û�ָ�����cfm���ܼ���: %s\r\n"), itUserSelCfm->second);
	}

	// ���Ĭ���������wsp�Ƿ�����������򿪲���Ԥ֪�Ĺ�����
	itUserSelCfm = m_mapUserLastSelCfm.find(KStrLastUserSelDefaultUserName);
	if ( itUserSelCfm != m_mapUserLastSelCfm.end() )
	{
		CfmInfoMap::const_iterator it = cfmInfos.find(itUserSelCfm->second);
		if ( it != cfmInfos.end() )
		{
			cfmInfo = it->second;
			TRACE(_T("�û�ָ�����cfm����Ĭ����: %s\r\n"), itUserSelCfm->second);
			return true;	// �ҵ�Ĭ�ϼ������Ϊ��������
		}
	}

	// ������ʱ�����
	// ֻ���������һ����
	// ���ʱ��������: �ȿ��û��Լ�Ŀ¼�ģ�Ȼ���ڱȽ�ϵͳĿ¼��
// 	CStringArray aSortedTimeWsps;
// 	SortByFileTime(cfmInfos, aSortedTimeWsps);
// 	{
// 		int i = 0;
// 		for ( i=0; i < aSortedTimeWsps.GetSize() ; i++ )
// 		{
// 			CfmInfoMap::const_iterator it = cfmInfos.find(aSortedTimeWsps[i]);
// 			ASSERT( it != cfmInfos.end() );
// 			if ( it != cfmInfos.end() && it->second.m_bInUserPrivateDir )// ˽��Ŀ¼�µ�����
// 			{
// 				cfmInfo = it->second;
// 				ASSERT( !cfmInfo.m_bSecret );	// Ӧ���������⹤����
// 				TRACE(_T("�û�ָ�����cfm����ʱ�������(�û�): %s\r\n"), cfmInfo.m_StrXmlName);
// 				return true;	// 
// 			}
// 		}
// 
// 		for ( i=0; i < aSortedTimeWsps.GetSize() ; i++ )
// 		{
// 			CfmInfoMap::const_iterator it = cfmInfos.find(aSortedTimeWsps[i]);
// 			ASSERT( it != cfmInfos.end() );
// 			if ( it != cfmInfos.end() && !it->second.m_bInUserPrivateDir )// �ֵ�ϵͳĿ¼�ˣ���ʵӦ��ȫ����ϵͳĿ¼��
// 			{
// 				cfmInfo = it->second;
// 				TRACE(_T("�û�ָ�����cfm����ʱ�������(ϵͳ): %s\r\n"), cfmInfo.m_StrXmlName);
// 				return true;	// 
// 			}
// 		}
// 	}

	// ��ʱ����Ҳ�����֤��û��һ�����������򿪵�Ĭ�Ϲ���ҳ�棬mainframe��Ҫ������������
	TRACE(_T("�û��޷�ƥ���κ�cfm: %s\r\n"), StrUserName);
	ASSERT( 0 );
	return false;
}

void CCfmManager::SetUserLastSelCfm( const CString &StrWspXmlName, const CString &StrUserName )
{
	if ( StrUserName.IsEmpty() )
	{
		return;			// ���û����ǲ���Ҫ�����, �����ڵ�¼ʱ����ѡ��wsp���ܻᵼ���������
	}
	ASSERT( StrUserName != KStrLastUserSelDefaultUserName );	// Ӧ���ǲ���������û�����

	// �ٶ�ָ����wsp��һ�����ڵģ������ٴ�����һ��
	// ��������û������ڣ���¼������ô���� - ���Ӧ���ڵ�¼��֤���ʱ��¼
	m_mapUserLastSelCfm[StrUserName] = StrWspXmlName;		// ����ʵ������û��治���ڣ���������¼����~

	SavePublicLastCfmList();	// ����
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
			for ( int i=0; i < aTimes.GetSize() ; i++ )	// û��ʱ����Ĳ��ڱȽϷ�Χ��
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
			aCfmXmlNames.Add(it->first);	// û��ʱ����������������~~
		}
	}
}


bool32 CCfmManager::AddNewUserCfm( const CString &StrXmlName, bool32 bUserCfm/*=true*/ )
{
	ASSERT( IsInitialized() );
	// ����һ���µĿյ�cfm
	// TODO ��Ϊ��Ҫ��һ��ȷ��
	T_CfmFileInfo cfmNew;
	cfmNew.m_StrXmlName = cfmNew.m_StrFileName = StrXmlName;
	cfmNew.m_StrFileName += GetProperFileExt(bUserCfm);//_T(".cfm");	// ֻҪ���½��Ķ����������
	if ( IsNewXmlNameConflict(StrXmlName) > EWCT_CanOverwrite )
	{
		ASSERT( 0 );	// ��ʱӦ��ǰ�������ɸѡ�� - cfm��������
		return false;	// xml���Ƴ�ͻ���޽�
	}

	if ( IsNewFileNameConflict(cfmNew.m_StrFileName) )
	{
		ASSERT( 0 );
		return false;	// �ļ����Ƴ�ͻ��cfm�޽�
	}
	
	cfmNew.m_bSystemDirHas = false;	// 
	cfmNew.m_bInUserPrivateDir = true;
	cfmNew.m_bReadOnly = false;
	cfmNew.m_bLockSplit = false;
	cfmNew.m_bSecret = false;
	cfmNew.m_iUpdateMode = 0;
	cfmNew.m_bNeedServerAuth = false;

	cfmNew.m_StrFileVersion = _T("1.0.0.1");		// �½��İ汾����ȷ�����Ǹ���cur����
	if ( QueryUserCfm(StrXmlName, cfmNew) )
	{
		// �������ǰ��wsp����̳�������
		// ���������ǰ��wsp��������xml������ͬ�������� file/version/m_bSystemWspDirHas Ӧ����ͬ
		ASSERT( !cfmNew.m_bSecret );	// ��Ӧ�������صĳ���
		DelUserCfm(StrXmlName);	// ɾ����ǰ��wsp
	}

	CGGTongDoc* pDoc = (CGGTongDoc*)AfxGetDocument();
	cfmNew.m_bInUserPrivateDir = true;	// �ƶ���˽��Ŀ¼��
	CString StrPath = CPathFactory::GetPrivateWorkspacePath(pDoc->m_pAbsCenterManager->GetUserName());
	StrPath += cfmNew.m_StrFileName;
	RelativePathToAbsPath(StrPath, cfmNew.m_StrFilePath);		// ���Ϊ˽��Ŀ¼��

	UpdateUserCfm(StrXmlName, cfmNew);

	return true;
}

int CCfmManager::CheckCfmCanAdd( INOUT T_CfmFileInfo &cfmInfo, const CfmInfoMap &infoMap, bool32 bEnableRename /*= false*/ )const
{
	const int cannotAdd = -1;
	const int addedDirect = 0;
	const int addedRename = 1;
	// ��ӹ���(һ�����):
	//	1. �Ƿ����ͬ����
	//	2. ͬ���Ļ����Ƿ����������������������������xml���� = xml_filename1(?)
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
				StrName = cfmInfo.m_StrXmlName + _T("[�û�]");
			}
			CString StrNameBase = StrName;
			int iStart = 1;
			while ( infoMap.count(StrName) )
			{
				// ���ظ�����ʹ��������
				StrNum.Format(_T("%d"), iStart);
				StrName =  StrNameBase + StrNum;
				iStart++;
			}
			cfmInfo.m_StrXmlName = StrName;	// ���µ�����
			return addedRename;
		}
		return cannotAdd;
	}
	return addedDirect;
}

void CCfmManager::RelativePathToAbsPath( const CString &StrRelative, OUT CString &StrAbs )
{
	// ���·��תΪ����·��
	StrAbs.Empty();
	if ( StrRelative.IsEmpty() )
	{
		return;
	}

	CString StrR = StrRelative;
	StrR.Replace(_T('\\'), _T('/'));
	if ( StrR.Find(_T(':')) == 1 )
	{
		StrAbs = StrR;		// �Ѿ��Ǿ���·��
		return;
	}

	// ./xxx ���� xxx/xxx
	// �ҳ������β

	CString StrTail;	// ����ǰ /
	int iPos = StrR.Find(_T("./"));
	if ( iPos == 0 )
	{
		StrTail = StrR.Mid(iPos+1);
	}
	else if ( StrR == _T(".") )	// ��ǰĿ¼
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
		return EWCT_Conflict;	// ��������ǰ����ֿ����ļ���bug����ʾһ��
	}

	// �����Ĺؼ���
	if ( (StrXmlName.Find(L".wsp")!=-1) 
		|| (StrXmlName.Find(L".cfm")!=-1) 
		|| (StrXmlName.Find(L".vmg")!=-1) 
		|| (StrXmlName.Find(L"Ĭ��")!=-1)
		|| (StrXmlName.Find(_T("system")) != -1)
		|| (StrXmlName.Find(AfxGetApp()->m_pszAppName) != -1)
		)
	{
		return EWCT_Conflict;
	}

	// cfm�������κ�ͬ��(�����ִ�Сд)
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
	// ��������û�Ŀ¼�µļ�¼
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
		//����Ҫ���
	}
	else //��Ҫ���ECUR_Restore
	{
		if( (dwRights&ECUR_Delete) == 0 && (dwRights&ECUR_Restore) == 0 )	// �޷�ɾ�����߻�ԭ
		{
			ASSERT( 0 );
			return;		// ��ǰwsp����ɾ����ϵͳ-�û�wsp����ɾ����ֻ�ǻ�ԭ����
		}
	}
	
	CfmInfoMap::iterator it = m_mapCfmInfoUser.find(StrXmlName);
	if ( it != m_mapCfmInfoUser.end() )
	{
		// ȡ�����ڵİ�
		CMPIChildFrame *pChildFrame = GetCfmFrame(StrXmlName);
		if ( NULL != pChildFrame )
		{
			BindCfm2ChildFrame(StrXmlName, pChildFrame, false);
		}

		::DeleteFile(it->second.m_StrFilePath);  // Ҫ��֤�����û�Ŀ¼�µ�
		m_mapCfmInfoUser.erase(it);

		// ɾ����֮����ܻᵼ��last...�Ȳ�ͬ��
	}
}

void CCfmManager::CurCfmSaveAs( const CString &StrSaveAsXmlName )
{
	ASSERT( IsInitialized() );

	// cfm��֧�ָ���
	T_CfmFileInfo cfm;
	if ( QueryUserCfm(StrSaveAsXmlName, cfm) )
	{
		ASSERT( 0 );
		return;
	}
	
	// ����ǰ�������Ϊҳ���ļ�, �����ǰ�����а󶨵����ԣ������Ա��Ϊ���Ϊ��ҳ������
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
				SaveCfm(StrId, pChildFrame);	// ����
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
		TRACE(_T("��ҳ����Ա��浽: %s\n"), StrXmlName);
		return;
	}

	T_CfmFileInfo cfm;
	if ( IsUserDefaultCfm(StrXmlName) )
	{
		// Ĭ�Ϲ���ҳ��
		cfm = m_cfmUserDefault;
	}
	else if ( !QueryUserCfm(StrXmlName, cfm) )
	{
		// �����ڣ�ֻ�ܳ����½�һ����
		AddNewUserCfm(StrXmlName);
		if ( !QueryUserCfm(StrXmlName, cfm) )
		{
			ASSERT( 0 );
			return;	// �޷���ȡwsp��Ϣ����֪����ô������
		}
	}

	if ( StrXmlName.IsEmpty() )
	{
		ASSERT( 0 );	// ���ļ�ҳ����ô������
		return;
	}
	
	if ( cfm.m_bReadOnly )
	{
		TRACE(_T("����ֻ������ҳ�汣��: %s\r\n"), cfm.m_StrFilePath);
		return;	// ���⹤�����Լ�ֻ�������������ڵ�������, ����ͨ����������棬��Ϊ���wsp�����Ѿ��������������
		// TODO �����ı������
	}

	// ����·��Ϊprivate��
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

	// ��������
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

	// ����version����
// 	T_CfmFileInfo cfmTest;
// 	ReadCfmInfoFromFile(cfm.m_StrFilePath, cfmTest);
// 	ASSERT( cfmTest.m_StrFileVersion == cfm.m_StrFileVersion );

	UpdateUserCfm(StrXmlName, cfm);	// ����������

#ifdef _DEBUG
	TRACE(_T("�ر�ҳ��[%s]: %d ms\r\n"), StrXmlName.operator LPCTSTR(), timeGetTime()-dwTime);
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
					// ����رյĴ��ڲ����Զ�����
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
	// ���ص�ǰ���ڰ󶨵�ҳ���ļ�, ����еĻ�
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
				return LoadCfm(StrId, true) != NULL;	// �˺��childframe���ر���
			}
		}
	}
	return false;
}

CMPIChildFrame * CCfmManager::LoadCfm( const CString &StrXmlName, bool32 bCloseExistence /*= false*/, bool32 bHideNewCfm/*=false*/, CString StrCfmUrl, bool bDelete )
{
	// ���Ϊ�ر��Ѿ����˸�ҳ���ļ��Ĵ��ڣ�Ȼ���½����ڲ���ָ��ҳ���ļ���

	ASSERT( IsInitialized() );
	T_CfmFileInfo cfm;
	
	if ( QueryUserCfm(StrXmlName, cfm) )
	{
		if ( cfm.m_bNeedServerAuth )		// ���Ȩ��
		{
			if ( !CPluginFuncRight::Instance().IsUserHasRight(StrXmlName, true) )
				return NULL;
		}

		ASSERT( !StrXmlName.IsEmpty() );
		
		CMainFrame *pMainFrame = (CMainFrame *)AfxGetMainWnd();
		ASSERT( pMainFrame != NULL && pMainFrame->IsKindOf(RUNTIME_CLASS(CMainFrame)) );
		if ( NULL != pMainFrame )
		{
			// TODO mainframeӦ�����������ҳ��
			//
			if ( _taccess(cfm.m_StrFilePath, 0) != 0 )
			{
				// ����������ļ���ֻ���滻��δ򿪲�����
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

			// �����Ѿ����˵ĸ�ҳ��, �ر���
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
						RDW_UPDATENOW | RDW_ALLCHILDREN | RDW_INVALIDATE);//ǿ���ػ棬�����Ӵ���
				}
				

				return pChildFrame;	// ���ر���ֱ�ӷ��� - �൱�ڴ���
			}
			else if ( bCloseExistence )
			{
				CloseCfmFrame(StrXmlName);
			}

			// ������cfm����			
			pChildFrame = (CMPIChildFrame *)pMainFrame->OpenChildFrame(cfm.m_StrXmlName, bHideNewCfm);
			ASSERT( NULL != pChildFrame );
		
			if ( NULL != pChildFrame )
			{
				BindCfm2ChildFrame(StrXmlName, pChildFrame);
				pMainFrame->MDIActivate(pChildFrame);
				// ������������ӵ���ǩ
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
				
				// �ر�������ϵͳҳ��
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
		return;		// ���д˱�־���û����������������
	}
	T_CfmFileInfo cfmTmp;
	if ( QueryUserCfm(StrXmlName, cfmTmp) )
	{
		if ( cfmTmp.m_bReadOnly )
		{
			return;	// �˲��ܸ������û��У� ������ϵͳ�Ļ����û���
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
	// Ӱ���Ǵӳ����´�������ʼ����Ӱ��˴�
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
	
	// ���Ƿ�����ļ���, ������ļ����ڷ�
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
		if ( !IsUserDefaultCfm(info.m_StrXmlName) )	// Ĭ��ҳ�治��ɾ��
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
	std::pair<DWORD, CString>(CCfmManager::ESDC_Report, _T("�����б�")),
		std::pair<DWORD, CString>(CCfmManager::ESDC_KLine, _T("K�߷���")),
		std::pair<DWORD, CString>(CCfmManager::ESDC_Trend, _T("��ʱ����")),
		std::pair<DWORD, CString>(CCfmManager::ESDC_News, _T("������Ѷ")),
		std::pair<DWORD, CString>(CCfmManager::ESDC_PhaseSort, _T("�׶�����")),
		std::pair<DWORD, CString>(CCfmManager::ESDC_KlineArbitrage, _T("����K��")),
		std::pair<DWORD, CString>(CCfmManager::ESDC_TrendArbitrage, _T("������ʱ")),
		std::pair<DWORD, CString>(CCfmManager::ESDC_ReportArbitrage, _T("�������۱�")),
		std::pair<DWORD, CString>(CCfmManager::ESDC_HomePage, _T("��ҳ")),
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
	// �ر�ָ������������κ�ϵͳcfm(Ĭ�� ͼ�����)
	int32 iClose = 0;
	CMainFrame *pMainFrame = (CMainFrame *)AfxGetMainWnd();
	ASSERT( pMainFrame != NULL && pMainFrame->IsKindOf(RUNTIME_CLASS(CMainFrame)) );

	
	// ����Ǳ�������Ҫ������ҳ��Ϳ��Բ��ر�����ҳ���ˣ���Ϊ��������ҳ������
	// ��Ҫ�����ļ���
	CStringArray aReserveName;
	GetReserveCfmNames(aReserveName);
	if ( IsReserveCfm(StrXmlNameReserve) )
	{// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		return 0;
	}

	if ( !StrXmlNameReserve.IsEmpty() )
	{
		aReserveName.Add(StrXmlNameReserve);	// ���Լ��ų�����
	}

	CStringArray aStrCaption;
	CNewCaptionTBWnd::GetCaptionArray(aStrCaption);
	for (int i = 0; i < aStrCaption.GetSize(); ++i)
	{	
		//--- wangyongxue ��������ֹ���
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
		if ( cfmReserve.m_bSystemDirHas )	// ϵͳ
		{
			QueryUserSystemCfm(mapDelCfms);
		}
		else
		{
			QueryUserPrivateCfm(mapDelCfms); // �û�
		}
	}
	else
	{
		// �ر����е�
		QueryUserAllCfm(mapDelCfms);
	}
	
	for ( int32 i=0; i < aReserveName.GetSize() ; i++ )
	{
		mapDelCfms.erase( aReserveName[i] );	// �Ƴ���Щ����������
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
					SaveCfm(StrId, pChild);	// �Զ����汻�رյ�
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
	// ��Ҫ�����ļ���
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
	// ��Ҫ�����ļ���
	CString StrTrend, StrKline, StrPhaseSort, StrReport;
	GetSystemDefaultCfmName(CCfmManager::ESDC_Trend, StrTrend);
	GetSystemDefaultCfmName(CCfmManager::ESDC_KLine, StrKline);
	GetSystemDefaultCfmName(CCfmManager::ESDC_PhaseSort, StrPhaseSort);
	GetSystemDefaultCfmName(CCfmManager::ESDC_Report, StrReport);
	aReserveName.Add(StrKline);
	aReserveName.Add(StrTrend);
	aReserveName.Add(StrPhaseSort);		// �׶�����ҳ�治�Զ��ر�
	aReserveName.Add(StrReport);
	aReserveName.Add(_T("����ͼ"));

	//	��������ҳ����¼�������Ҫ���¼��ذ�������
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
		return _T(".cfu");	// �û��Ĺ���ҳ���ļ���׺��
	}
	else
	{
		return _T(".cfm"); // ϵͳ�Ĺ���ҳ���ļ��ĺ�׺��
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
