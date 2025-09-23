#include "stdafx.h"
#include "XmlShare.h"

#include "PathFactory.h"
#include "coding.h"
#include "ShareFun.h"
#include "tinyxml.h"
#include <time.h>




#include <crtdbg.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// �ͻ���ϵͳ����xml�ļ���ͳһ���ڵ���عؼ���
static const char * KStrXmlRootElement				= "XMLDATA";
static const char * KStrXmlRootElementAttrApp		= "app";
static const char * KStrXmlRootElementAttrAppValue	= "ggtong";
static const char * KStrXmlRootElementAttrVersion	= "version";
static const char * KStrXmlRootElementAttrData		= "data";
static const char * KStrXmlRootElementAttrCodeType = "codetype";
static const char * KStrXmlRootElementAttrReadOnly = "readonly";
static const char * KStrXmlRootElementAttrVersionValue		= "1.0.0.1";
static const char * KStrXmlRootElementAttrCodeTypeValue	= "0";					// Ĭ���޴��ֶ�
static const char * KStrXmlRootElementAttrReadOnlyValue	= "0";					// Ĭ�Ͽɶ�д


TiXmlElement		* ConstructGGTongAppXmlDocHeader( INOUT TiXmlDocument &tiDoc, const char *pcszDataName,
													 const char *pcszVersionValue, const char *pcszCodeType, const char *pcszReadOnly )
{
	// ����Ҫ�ǿյ�tiDoc��������ִ���
	ASSERT( tiDoc.FirstChildElement() == NULL );
	tiDoc.Clear();		// ��ǿ�������������
	
	if ( NULL == pcszVersionValue || pcszVersionValue[0] == '\0' )
	{
		pcszVersionValue = KStrXmlRootElementAttrVersionValue;
	}
	if ( NULL == pcszDataName || pcszDataName[0] == '\0' )
	{
		pcszDataName = KStrXmlRootElementAttrAppValue;
	}
	if ( NULL == pcszCodeType || pcszCodeType[0] == '\0' )
	{
		pcszCodeType = NULL;	// ��ָ��
	}
	if ( NULL == pcszReadOnly || pcszReadOnly[0] == '\0' )
	{
		pcszReadOnly = KStrXmlRootElementAttrReadOnlyValue;
	}
	
	// ��Ӹ��ڵ�
	TiXmlDeclaration	tiDecl("1.0", "utf-8", "yes");
	tiDoc.InsertEndChild(tiDecl);
	
	TiXmlElement	tiEleRoot(GetXmlRootElement());
	tiEleRoot.SetAttribute(GetXmlRootElementAttrVersion(), pcszVersionValue);
	tiEleRoot.SetAttribute(GetXmlRootElementAttrApp(),	   KStrXmlRootElementAttrAppValue);
	tiEleRoot.SetAttribute(GetXmlRootElementAttrData(),	   pcszDataName);
	if ( NULL != pcszCodeType )
	{
		tiEleRoot.SetAttribute(GetXmlRootElementAttrCodeType(),    pcszCodeType);
	}
	tiEleRoot.SetAttribute(GetXmlRootElementAttrReadOnly(),	   pcszReadOnly);
	TiXmlElement *pRoot = (TiXmlElement *)tiDoc.InsertEndChild(tiEleRoot);
	ASSERT( NULL != pRoot );
	return pRoot;
}

bool32 GetLocalDayGmtRange( IN CGmtTime TimeDay, OUT CGmtTime &TimeDayLocalStart, OUT CGmtTime &TimeDayLocalEnd )
{
	tm *pLocal = TimeDay.GetLocalTm();
	if ( NULL != pLocal )
	{
		long tDiff = _timezone;
		time_t tGmtDay = TimeDay.GetTime();
		time_t tLocalDay = tGmtDay + tDiff;		// ����ʱ����gmtʱ������ʾ
		TimeDayLocalStart = tLocalDay;
		SaveDay(TimeDayLocalStart);
		int64 tmpTime = int64(TimeDayLocalStart.GetTime() - (int64)(2*tDiff));
		TimeDayLocalStart = CGmtTime((LONGLONG)tmpTime);;	// ��ԭ�ɱ����쿪ʼ����ʾ��ʵ��gmtʱ��
		TimeDayLocalEnd = TimeDayLocalStart + CGmtTimeSpan(0, 23, 59, 59);
		return true;
	}
	else
	{
		// �Ѿ�������, ����Ĭ�ϵ�gmtʱ��
		ASSERT( 0 );
		TimeDayLocalStart = TimeDay;
		SaveDay(TimeDayLocalStart);
		TimeDayLocalEnd = TimeDayLocalStart + CGmtTimeSpan(0, 23, 59, 59);
	}
	return false;
}

bool LoadTiXmlDoc( INOUT TiXmlDocument &doc,  LPCTSTR lptszFile, TiXmlEncoding enCode/*=TIXML_DEFAULT_ENCODING*/ )
{
#ifndef UNICODE
	// MBCS
	return doc.LoadFile(lptszFile);
#else
	// UNICODE
	bool bRet = false;
	FILE *pf = _tfopen(lptszFile, _T("rb"));
	if ( NULL!=pf )
	{
		bRet = doc.LoadFile(pf, enCode);
		fclose(pf);
		pf = NULL;
	}
	return bRet;
#endif
}

bool SaveTiXmlDoc( const TiXmlDocument &doc, LPCTSTR lptszFile )
{
#ifndef UNICODE
	// MBCS
	return doc.SaveFile(lptszFile);
#else
	// UNICODE
	bool bRet = false;
	FILE *pf = _tfopen(lptszFile, _T("w"));
	if ( NULL!=pf )
	{
		bRet = doc.SaveFile(pf);
		fclose(pf);
		pf = NULL;
	}
	return bRet;
#endif
}

//////////////////////////////////////////////////////////////////////////
// ��������
const char KStrDefaultSection[] = "EtcConfigDefault";
const char KStrXmlAttriValue[]	= "v";
const char KStrRightIndexNameSection[] = "UserRightIndexName";

CEtcXmlConfig::CEtcXmlConfig()
{
	m_pRootPrivate = NULL;
	m_pRootPublic = NULL;
}

bool32 CEtcXmlConfig::Initialize()
{
	LoadTiXmlDoc(m_TiDocPrivate, GetPrivateFileName());
	m_pRootPrivate = m_TiDocPrivate.RootElement();
	if ( NULL == m_pRootPrivate )
	{
		// �յ�˽��
		m_pRootPrivate = ConstructGGTongAppXmlDocHeader(m_TiDocPrivate, "EtcConfig", NULL, NULL, NULL);	// ����һ���յ�
	}
	
	LoadTiXmlDoc(m_TiDocPublic, GetPublicFileName());
	m_pRootPublic = m_TiDocPublic.RootElement();
	if ( NULL == m_pRootPublic )
	{
		m_pRootPublic = ConstructGGTongAppXmlDocHeader(m_TiDocPublic, "EtcConfig", NULL, NULL, NULL);	// ����һ���յ�
	}
	// ���е�û��Ĭ���ļ���

	return m_pRootPrivate != NULL;
}

CEtcXmlConfig & CEtcXmlConfig::Instance()
{
	static CEtcXmlConfig config;
	static bool32 bInit = false;
	if ( !bInit )
	{
		config.Initialize();		// ��Ĭ�ϵ�һ�γ�ʼ��
		bInit = true;
	}
	return config;
}

char * CEtcXmlConfig::WideToMulti( LPCWSTR pwszSrc, UINT uCodePage /*=CP_ACP*/ )
{
	if ( NULL == pwszSrc )
	{
		return NULL;
	}
	
	const int32 iLen = ::WideCharToMultiByte(uCodePage, 0, pwszSrc, -1, NULL, 0, NULL, NULL);
	if ( iLen <= 0 )
	{
		return NULL;
	}
	char *psz = new char[iLen];
	if ( NULL == psz )
	{
		return NULL;
	}
	const int32 iR = ::WideCharToMultiByte(uCodePage, 0, pwszSrc, -1, psz, iLen, NULL, NULL);
	if ( iR != iLen )
	{
		ASSERT( 0 );
		delete []psz;
		return NULL;
	}
	
	return psz;
}

wchar_t * CEtcXmlConfig::MultiToWide( const char *pszSrc, UINT uCodePage /*= CP_ACP*/ )
{
	if ( NULL == pszSrc )
	{
		return NULL;
	}
	
	const int32 iLen = ::MultiByteToWideChar(uCodePage, 0, pszSrc, -1, NULL, 0);
	if ( iLen <= 0 )
	{
		return NULL;
	}
	wchar_t *pwsz = new wchar_t[iLen];
	if ( NULL == pwsz )
	{
		return NULL;
	}
	const int32 iR = ::MultiByteToWideChar(uCodePage, 0, pszSrc, -1, pwsz, iLen);
	if ( iR != iLen )
	{
		ASSERT( 0 );
		delete []pwsz;
		return NULL;
	}
	return pwsz;
}

bool32 CEtcXmlConfig::ReadEtcConfig( const char *pszSection, const char *pszKey, const char *pszDefault, OUT string &StrValue )
{
	if ( NULL == m_pRootPrivate )
	{
		return false;	// �޷�����
	}

	if ( NULL == pszKey || '\0' == pszKey[0] )
	{
		return false;	// �޷���ȡ
	}

	if ( NULL == pszSection || '\0' == pszSection[0] )
	{
		pszSection = KStrDefaultSection;	// Ĭ��ҳ��
	}

	bool32 bRead = ReadXmlContent(pszSection, pszKey, StrValue);

	if ( !bRead && NULL != pszDefault )
	{
		StrValue = pszDefault;
		bRead = true;		// ����Ĭ�ϵ�
	}

	return bRead;
}

bool32 CEtcXmlConfig::ReadEtcConfig( LPCWSTR pwszSection, LPCWSTR pwszKey, LPCWSTR pwszDefault, OUT CString &StrValue )
{
	if ( NULL == m_pRootPrivate )
	{
		return false;	// �޷�����
	}
	
	if ( NULL == pwszKey || L'\0' == pwszKey[0] )
	{
		return false;	// �޷���ȡ
	}

	char *pszSection = NULL;
	if ( NULL != pwszSection && pwszSection[0] != L'\0' )
	{
		pszSection = WideToMulti(pwszSection, CP_UTF8);
	}
	
	char *pszKey = WideToMulti(pwszKey, CP_UTF8);
	char *pszDefault = NULL;
	if ( NULL != pwszDefault )
	{
		pszDefault = WideToMulti(pwszDefault, CP_UTF8);
	}
	
	string StrByteValue;
	bool32 bRet = ReadEtcConfig(pszSection, pszKey, pszDefault, StrByteValue);
	if ( bRet )
	{
		wchar_t *pwsz = MultiToWide(StrByteValue.c_str(), CP_UTF8);
		StrValue = pwsz;
		delete []pwsz;
	}

	delete []pszSection;
	delete []pszKey;
	delete []pszDefault;

	return bRet;
}

bool32 CEtcXmlConfig::WriteEtcConfig( const char *pszSection, const char *pszKey, const char *pszValue )
{
	if ( NULL == m_pRootPrivate )
	{
		return false;	// �޷�����
	}
	// ����ʲôsection��ʲôkey��������ɾ��
	if ( NULL == pszSection || '\0' == pszSection[0] )
	{
		pszSection = KStrDefaultSection;
	}

	if ( NULL == pszKey || '\0' == pszKey[0] )
	{
		// ɾ����section, 
		TiXmlElement *pSection = m_pRootPrivate->FirstChildElement(pszSection);
		if ( NULL != pSection )
		{
			m_pRootPrivate->RemoveChild(pSection);
			SaveTiXmlDoc(m_TiDocPrivate, GetPrivateFileName());	// ����
		}
		return true;	// over
	}

	if ( NULL == pszValue || '\0' == pszValue[0] )
	{
		// ɾ����section��key
		TiXmlElement *pSection = m_pRootPrivate->FirstChildElement(pszSection);
		if ( NULL != pSection )
		{
			TiXmlElement *pKey = pSection->FirstChildElement(pszKey);
			bool32 bDel = false;
			if ( NULL != pKey )
			{
				pSection->RemoveChild(pKey);
				bDel = true;
			}
			if ( pSection->FirstChildElement() == NULL )
			{
				m_pRootPrivate->RemoveChild(pSection);	// ������key���Ƴ�section
				bDel = true;
			}
			if ( bDel )
			{
				SaveTiXmlDoc(m_TiDocPrivate, GetPrivateFileName());
			}
		}
		return true;	// over
	}

	// ����ֵ
	// ���Ҫ���õ�ֵ�빲�е�ֵ��һ���ģ���ɾ����˽�е�ֵ, �����Ժ��滻����Ĭ��ֵ, �Ƿ�Ҫʵ�ָù��ܣ�����
	string strPublicV;
	bool32 bPublic = NULL!=m_pRootPublic ? ReadXmlContent(m_pRootPublic, pszSection, pszKey, strPublicV) : false;
	if ( bPublic && strPublicV.compare(pszValue) == 0 )
	{
		// �Ƴ���ֵ
		TiXmlElement *pSection = m_pRootPrivate->FirstChildElement(pszSection);
		if ( NULL != pSection )
		{
			TiXmlElement *pKey = pSection->FirstChildElement(pszKey);
			bool32 bDel = false;
			if ( NULL != pKey )
			{
				pSection->RemoveChild(pKey);
				bDel = true;
			}
			if ( pSection->FirstChildElement() == NULL )
			{
				m_pRootPrivate->RemoveChild(pSection);	// ������key���Ƴ�section
				bDel = true;
			}
			if ( bDel )
			{
				SaveTiXmlDoc(m_TiDocPrivate, GetPrivateFileName());
			}
		}
		return true;	// over
	}

	TiXmlElement *pSection = m_pRootPrivate->FirstChildElement(pszSection);
	if ( NULL == pSection )
	{
		TiXmlElement tiEle(pszSection);
		pSection = (TiXmlElement *)m_pRootPrivate->InsertEndChild(tiEle);
		if ( NULL == pSection )
		{
			return false;
		}
	}
	TiXmlElement *pKey = pSection->FirstChildElement(pszKey);
	if ( NULL == pKey )
	{
		TiXmlElement tiKey(pszKey);
		pKey = (TiXmlElement *)pSection->InsertEndChild(tiKey);
		if ( NULL == pKey )
		{
			return false;
		}
	}

	pKey->SetAttribute(KStrXmlAttriValue, pszValue);

	SaveTiXmlDoc(m_TiDocPrivate, GetPrivateFileName());
	
	return true;
}

bool32 CEtcXmlConfig::WriteEtcConfig( LPCWSTR pwszSection, LPCWSTR pwszKey, LPCWSTR pwszValue )
{
	if ( NULL == m_pRootPrivate )
	{
		return false;	// �޷�����
	}

	char *pszSection = NULL;	// Null���ֵ������
	if ( NULL != pwszSection && pwszSection[0] != L'\0' )
	{
		pszSection = WideToMulti(pwszSection, CP_UTF8);
		if ( NULL == pszSection )
		{
			return false;
		}
	}
	
	char *pszKey = NULL;
	if ( NULL != pwszKey && pwszKey[0] != L'\0' )
	{
		pszKey = WideToMulti(pwszKey, CP_UTF8);
		if ( NULL == pszKey )
		{
			DEL_ARRAY(pszSection);
			return false;
		}
	}

	char *pszValue = NULL;
	if ( NULL != pwszValue && pwszValue[0] != L'\0' )
	{
		pszValue = WideToMulti(pwszValue, CP_UTF8);
		if ( NULL == pszValue )
		{
			DEL_ARRAY(pszSection);
			DEL_ARRAY(pszKey);
		}
	}

	bool32 bRet = FALSE;
	if(pszSection && pszKey)
	{
		bRet = WriteEtcConfig(pszSection, pszKey, pszValue);
	}

	delete []pszSection;
	delete []pszKey;
	delete []pszValue;

	return bRet;
}

bool32 CEtcXmlConfig::ReadXmlContent( TiXmlElement *pRoot, const char *pszSection, const char *pszKey, OUT string &StrValue )
{
	ASSERT( NULL != pRoot );
	ASSERT( NULL != pszSection );
	ASSERT( NULL != pszKey );

	bool32 bRead = false;
	TiXmlElement *pSection = pRoot->FirstChildElement(pszSection);
	if ( NULL != pSection )
	{
		TiXmlElement *pKey = pSection->FirstChildElement(pszKey);
		if ( NULL != pKey )
		{
			const char *pValue = pKey->Attribute(KStrXmlAttriValue);
			if ( NULL != pValue )
			{
				StrValue = pValue;
				bRead = true;
			}
		}
	}
	return bRead;
}

bool32 CEtcXmlConfig::ReadXmlContent( const char *pszSection, const char *pszKey, OUT string &StrValue )
{
	// ��Private��public
	if ( NULL != m_pRootPrivate
		&& ReadXmlContent(m_pRootPrivate, pszSection, pszKey, StrValue) )
	{
		return true;
	}

	if ( NULL != m_pRootPublic
		&& ReadXmlContent(m_pRootPublic, pszSection, pszKey, StrValue) )
	{
		return true;
	}

	return false;
}

static const CString s_StrFileName(_T("etcConfig.xml"));
CString CEtcXmlConfig::GetPrivateFileName() const
{

	CGGTongDoc* pDoc = (CGGTongDoc*)AfxGetDocument();
	return CPathFactory::GetPrivateConfigPath(pDoc->m_pAbsCenterManager->GetUserName()) + s_StrFileName;
}

CString CEtcXmlConfig::GetPublicFileName() const
{
	return CPathFactory::GetPublicConfigPath() + s_StrFileName;
}

