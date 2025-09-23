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

// 客户端系统所有xml文件的统一根节点相关关键字
static const char * KStrXmlRootElement				= "XMLDATA";
static const char * KStrXmlRootElementAttrApp		= "app";
static const char * KStrXmlRootElementAttrAppValue	= "ggtong";
static const char * KStrXmlRootElementAttrVersion	= "version";
static const char * KStrXmlRootElementAttrData		= "data";
static const char * KStrXmlRootElementAttrCodeType = "codetype";
static const char * KStrXmlRootElementAttrReadOnly = "readonly";
static const char * KStrXmlRootElementAttrVersionValue		= "1.0.0.1";
static const char * KStrXmlRootElementAttrCodeTypeValue	= "0";					// 默认无此字段
static const char * KStrXmlRootElementAttrReadOnlyValue	= "0";					// 默认可读写


TiXmlElement		* ConstructGGTongAppXmlDocHeader( INOUT TiXmlDocument &tiDoc, const char *pcszDataName,
													 const char *pcszVersionValue, const char *pcszCodeType, const char *pcszReadOnly )
{
	// 必须要是空的tiDoc，避免出现错误
	ASSERT( tiDoc.FirstChildElement() == NULL );
	tiDoc.Clear();		// 会强制清除所有数据
	
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
		pcszCodeType = NULL;	// 不指定
	}
	if ( NULL == pcszReadOnly || pcszReadOnly[0] == '\0' )
	{
		pcszReadOnly = KStrXmlRootElementAttrReadOnlyValue;
	}
	
	// 添加根节点
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
		time_t tLocalDay = tGmtDay + tDiff;		// 本地时间用gmt时间来表示
		TimeDayLocalStart = tLocalDay;
		SaveDay(TimeDayLocalStart);
		int64 tmpTime = int64(TimeDayLocalStart.GetTime() - (int64)(2*tDiff));
		TimeDayLocalStart = CGmtTime((LONGLONG)tmpTime);;	// 还原成本地天开始数表示的实际gmt时间
		TimeDayLocalEnd = TimeDayLocalStart + CGmtTimeSpan(0, 23, 59, 59);
		return true;
	}
	else
	{
		// 已经出错了, 给与默认的gmt时间
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
// 杂项配置
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
		// 空的私有
		m_pRootPrivate = ConstructGGTongAppXmlDocHeader(m_TiDocPrivate, "EtcConfig", NULL, NULL, NULL);	// 构建一个空的
	}
	
	LoadTiXmlDoc(m_TiDocPublic, GetPublicFileName());
	m_pRootPublic = m_TiDocPublic.RootElement();
	if ( NULL == m_pRootPublic )
	{
		m_pRootPublic = ConstructGGTongAppXmlDocHeader(m_TiDocPublic, "EtcConfig", NULL, NULL, NULL);	// 构建一个空的
	}
	// 共有的没有默认文件名

	return m_pRootPrivate != NULL;
}

CEtcXmlConfig & CEtcXmlConfig::Instance()
{
	static CEtcXmlConfig config;
	static bool32 bInit = false;
	if ( !bInit )
	{
		config.Initialize();		// 仅默认的一次初始化
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
		return false;	// 无法操作
	}

	if ( NULL == pszKey || '\0' == pszKey[0] )
	{
		return false;	// 无法读取
	}

	if ( NULL == pszSection || '\0' == pszSection[0] )
	{
		pszSection = KStrDefaultSection;	// 默认页面
	}

	bool32 bRead = ReadXmlContent(pszSection, pszKey, StrValue);

	if ( !bRead && NULL != pszDefault )
	{
		StrValue = pszDefault;
		bRead = true;		// 赋予默认的
	}

	return bRead;
}

bool32 CEtcXmlConfig::ReadEtcConfig( LPCWSTR pwszSection, LPCWSTR pwszKey, LPCWSTR pwszDefault, OUT CString &StrValue )
{
	if ( NULL == m_pRootPrivate )
	{
		return false;	// 无法操作
	}
	
	if ( NULL == pwszKey || L'\0' == pwszKey[0] )
	{
		return false;	// 无法读取
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
		return false;	// 无法操作
	}
	// 不论什么section，什么key，都可以删除
	if ( NULL == pszSection || '\0' == pszSection[0] )
	{
		pszSection = KStrDefaultSection;
	}

	if ( NULL == pszKey || '\0' == pszKey[0] )
	{
		// 删除该section, 
		TiXmlElement *pSection = m_pRootPrivate->FirstChildElement(pszSection);
		if ( NULL != pSection )
		{
			m_pRootPrivate->RemoveChild(pSection);
			SaveTiXmlDoc(m_TiDocPrivate, GetPrivateFileName());	// 保存
		}
		return true;	// over
	}

	if ( NULL == pszValue || '\0' == pszValue[0] )
	{
		// 删除该section下key
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
				m_pRootPrivate->RemoveChild(pSection);	// 旗下无key，移除section
				bDel = true;
			}
			if ( bDel )
			{
				SaveTiXmlDoc(m_TiDocPrivate, GetPrivateFileName());
			}
		}
		return true;	// over
	}

	// 设置值
	// 如果要设置的值与共有的值是一样的，则删除该私有的值, 方便以后替换更改默认值, 是否要实现该功能？？？
	string strPublicV;
	bool32 bPublic = NULL!=m_pRootPublic ? ReadXmlContent(m_pRootPublic, pszSection, pszKey, strPublicV) : false;
	if ( bPublic && strPublicV.compare(pszValue) == 0 )
	{
		// 移除该值
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
				m_pRootPrivate->RemoveChild(pSection);	// 旗下无key，移除section
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
		return false;	// 无法操作
	}

	char *pszSection = NULL;	// Null与空值等意义
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
	// 先Private后public
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

