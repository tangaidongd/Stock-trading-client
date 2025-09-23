#include "StdAfx.h"

#include "tinyxml.h"
#include "PathFactory.h"
#include "coding.h"
#include <string>
using std::wstring;

#include "FontInstall.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
//
static const TCHAR KStrXMLFileName[]				=	_T("font.xml");
static const char KStrXMLRootName[]					=	"XMLDATA"; // ���ڵ�
static const char KStrXMLRootElementVersion[]		=	"version";			// ���ڵ� �汾
static const char KStrXMLRootElementApp[]			=	"app";			// ���ڵ� app
static const char KStrXMLRootElementData[]			=	"data";			// ���ڵ� data

static const char KStrXMLRootElementVersionDef[]	=	"1.0";			// ���ڵ� �汾 Ĭ��ֵ
static const char KStrXMLRootElementAppDef[]		=	"ggtong";			// ���ڵ� app default
static const char KStrXMLRootElementDataDef[]		=	"font";			// ���ڵ� data default

static const char KStrXMLElementNodeFont[]			=	"font";		// ���弸��
static const char KStrXMLElementAttriName[]			=	"name";		// ��������	- ��������
static const char KStrXMLElementAttriFile[]			=	"file";		// �汾������ - ����Ҫ��װ�������ļ�

CFontInstallHelper::CFontInstallHelper()
{
	LoadFromXml();
	AddFonts();
}

CFontInstallHelper::~CFontInstallHelper()
{
	RemoveFonts();
}

int CFontInstallHelper::AddFonts()
{
	int iInstalled = 0;
	for ( int i=0; i < m_aToInstallFontFiles.GetSize() ; i++ )
	{
		CString StrPath = CPathFactory::GetFontPath() + m_aToInstallFontFiles[i];
		if ( AddFontResource(StrPath) )		// WINVER����̫�ͣ�����ʹ��ex
		{
			m_aInstalledFontFiles.Add(StrPath);
			iInstalled++;
		}
	}
	m_aToInstallFontFiles.RemoveAll();
	if ( iInstalled > 0 )
	{
		::SendMessage(HWND_BROADCAST, WM_FONTCHANGE, 0, 0);
	}
	return iInstalled;
}

void CFontInstallHelper::RemoveFonts()
{
	for ( int i=0; i < m_aInstalledFontFiles.GetSize() ; i++ )
	{
		RemoveFontResource(m_aInstalledFontFiles[i]);
	}
	if ( m_aInstalledFontFiles.GetSize() > 0 )
	{
		::SendMessage(HWND_BROADCAST, WM_FONTCHANGE, 0, 0);
	}
	m_aInstalledFontFiles.RemoveAll();
}

void CFontInstallHelper::InstallFonts()
{
	static CFontInstallHelper helper;		// �᲻�ᱻ�Ż����˵�������
}

void CFontInstallHelper::SaveDefaultXML()
{
	CString StrPath = GetXmlFileName() + _T(".bak");
	
	TiXmlDocument tiDoc;
	
	// ��Ӹ��ڵ�
	TiXmlDeclaration	tiDecl("1.0", "utf-8", "yes");
	tiDoc.InsertEndChild(tiDecl);
	
	TiXmlElement	tiEleRoot(KStrXMLRootName);
	tiEleRoot.SetAttribute(KStrXMLRootElementVersion, KStrXMLRootElementVersionDef);
	tiEleRoot.SetAttribute(KStrXMLRootElementApp,	   KStrXMLRootElementAppDef);
	tiEleRoot.SetAttribute(KStrXMLRootElementData,    KStrXMLRootElementDataDef);
	TiXmlElement *pRoot = (TiXmlElement *)tiDoc.InsertEndChild(tiEleRoot);
	ASSERT( NULL != pRoot );
	
	// ���� s
	for ( int i=0; i < 3 ; i++ )
	{
		TiXmlElement	tiEleVer(KStrXMLElementNodeFont);
		TiXmlElement *pEle = (TiXmlElement *)pRoot->InsertEndChild(tiEleVer);
		pEle->SetAttribute(KStrXMLElementAttriName, "TODO:font name");
		pEle->SetAttribute(KStrXMLElementAttriFile, "TODO:font file");
	}
	
	char buf[MAX_PATH];
	
	::WideCharToMultiByte(CP_OEMCP, 0, StrPath, -1, buf, sizeof(buf), NULL, NULL);
	
	tiDoc.SaveFile(buf);
}

CString CFontInstallHelper::GetXmlFileName()
{
	return CPathFactory::GetPublicConfigPath() + KStrXMLFileName;
}

void CFontInstallHelper::LoadFromXml()
{
	m_aToInstallFontFiles.RemoveAll();


	TiXmlDocument	tiDoc;
	CString StrPath = GetXmlFileName();
	char buf[MAX_PATH];
	::WideCharToMultiByte(CP_OEMCP, 0, StrPath, -1, buf, sizeof(buf), NULL, NULL);
	if ( !tiDoc.LoadFile(buf) )
	{
		return;
	}
	
	TiXmlElement *pRoot = tiDoc.FirstChildElement(KStrXMLRootName);
	if ( NULL == pRoot )
	{
		return;
	}
	
	// ���xml�汾��Ϣ�� TODO
	
	// ����xml
	const char *pszAttr = NULL;
	TiXmlElement *pEle = pRoot->FirstChildElement(KStrXMLElementNodeFont);
	while ( NULL != pEle )
	{
		pszAttr = pEle->Attribute(KStrXMLElementAttriFile);	// �ļ�
		if ( NULL != pszAttr && pszAttr[0] != '\0' )
		{
			wstring	str;
			MultiChar2Unicode(CP_UTF8, pszAttr, str);
			m_aToInstallFontFiles.Add(str.c_str());
		}
		
		pEle = pEle->NextSiblingElement(KStrXMLElementNodeFont);
	}
	
}
