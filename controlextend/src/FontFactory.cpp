// FontFactory.cpp: implementation of the CFontFactory class.
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "FontFactory.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

vector<CString> CFontFactory::m_vFontName;
BYTE CFontFactory::m_bIsEnumFont = false;
CFontFactory gFontFactory;	// 全局的字体工厂变量
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFontFactory::CFontFactory()
{
	m_pFontnName = NULL;
	/** 枚举系统所有字体名称 */
	CFontFactory::GetEnumSystemFont();	
}

CFontFactory::~CFontFactory()
{
	ReleaseFontName(m_pFontnName);
	m_pFontnName = NULL;
}

WCHAR* CFontFactory::GetExistFontName(WCHAR *pFontName)
{
	if(!AssertFontIsExist(pFontName))
	{
		ReleaseFontName(m_pFontnName);
		int nLen = (1 + wcslen(DEFAULT_FONT)) * sizeof(WCHAR);
		m_pFontnName = new WCHAR[nLen];
		memset(m_pFontnName, 0, nLen);
		wcscpy(m_pFontnName, DEFAULT_FONT);

		return m_pFontnName;
	}

	return pFontName;
}

BOOL CFontFactory::AssertFontIsExist(const WCHAR *pFontName)
{
	/* 判断指定字体是否存在 */
	CString strTemp(pFontName);
	for(unsigned int i = 0; i < m_vFontName.size(); i++)
	{
		if(strTemp == m_vFontName[i])
		{
			return true;
		}
	}

	return false;
}
/*回调函数，枚举系统已经安装的字体*/ 
BOOL CALLBACK AFX_EXPORT CFontFactory::EnumFamScreenCallBackEx(ENUMLOGFONTEX *pelf,NEWTEXTMETRICEX *lpntm,int FontType,LPVOID pThis)
{
	CString str = ((pelf)->elfLogFont.lfFaceName);

	for(unsigned int i = 0; i < m_vFontName.size(); i++)
	{
		if(str == m_vFontName[i])
		{
			return TRUE;
		}
	}
	
	m_vFontName.push_back(str);
	
	return TRUE;
}

void CFontFactory::GetEnumSystemFont()
{
	if(m_bIsEnumFont) return ;

	HDC hDC = ::GetWindowDC(NULL);
	LOGFONT lf;
	
	memset(&lf,0,sizeof(LOGFONT));
	
	lf.lfCharSet = DEFAULT_CHARSET;	//GB2312_CHARSET;
	
	if (!::EnumFontFamiliesEx(hDC, &lf, (FONTENUMPROC)EnumFamScreenCallBackEx, (LPARAM)NULL/*this*/, (DWORD) 0))
	{
		return;
	}
	::ReleaseDC(NULL,hDC);

	m_bIsEnumFont = true;
}