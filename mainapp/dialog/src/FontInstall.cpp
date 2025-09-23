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
static const char KStrXMLRootName[]					=	"XMLDATA"; // 根节点
static const char KStrXMLRootElementVersion[]		=	"version";			// 根节点 版本
static const char KStrXMLRootElementApp[]			=	"app";			// 根节点 app
static const char KStrXMLRootElementData[]			=	"data";			// 根节点 data

static const char KStrXMLRootElementVersionDef[]	=	"1.0";			// 根节点 版本 默认值
static const char KStrXMLRootElementAppDef[]		=	"ggtong";			// 根节点 app default
static const char KStrXMLRootElementDataDef[]		=	"font";			// 根节点 data default

static const char KStrXMLElementNodeFont[]			=	"font";		// 字体几点
static const char KStrXMLElementAttriName[]			=	"name";		// 名称属性	- 仅描述用
static const char KStrXMLElementAttriFile[]			=	"file";		// 版本号属性 - 具体要安装的字体文件

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
		if ( AddFontResource(StrPath) )		// WINVER定义太低，不能使用ex
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
	static CFontInstallHelper helper;		// 会不会被优化过滤掉哈？？
}

void CFontInstallHelper::SaveDefaultXML()
{
	CString StrPath = GetXmlFileName() + _T(".bak");
	
	TiXmlDocument tiDoc;
	
	// 添加根节点
	TiXmlDeclaration	tiDecl("1.0", "utf-8", "yes");
	tiDoc.InsertEndChild(tiDecl);
	
	TiXmlElement	tiEleRoot(KStrXMLRootName);
	tiEleRoot.SetAttribute(KStrXMLRootElementVersion, KStrXMLRootElementVersionDef);
	tiEleRoot.SetAttribute(KStrXMLRootElementApp,	   KStrXMLRootElementAppDef);
	tiEleRoot.SetAttribute(KStrXMLRootElementData,    KStrXMLRootElementDataDef);
	TiXmlElement *pRoot = (TiXmlElement *)tiDoc.InsertEndChild(tiEleRoot);
	ASSERT( NULL != pRoot );
	
	// 字体 s
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
	
	// 检查xml版本信息等 TODO
	
	// 解析xml
	const char *pszAttr = NULL;
	TiXmlElement *pEle = pRoot->FirstChildElement(KStrXMLElementNodeFont);
	while ( NULL != pEle )
	{
		pszAttr = pEle->Attribute(KStrXMLElementAttriFile);	// 文件
		if ( NULL != pszAttr && pszAttr[0] != '\0' )
		{
			wstring	str;
			MultiChar2Unicode(CP_UTF8, pszAttr, str);
			m_aToInstallFontFiles.Add(str.c_str());
		}
		
		pEle = pEle->NextSiblingElement(KStrXMLElementNodeFont);
	}
	
}
