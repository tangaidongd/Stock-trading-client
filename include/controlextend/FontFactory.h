// FontFactory.h: interface for the CFontFactory class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FONTFACTORY_H__F6977B13_EA7B_4E4A_B4AC_C855002DC638__INCLUDED_)
#define AFX_FONTFACTORY_H__F6977B13_EA7B_4E4A_B4AC_C855002DC638__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <VECTOR>
#include "dllexport.h"
//#include "facescheme.h"

using namespace std;

#define DEFAULT_FONT		L"宋体"
#define Release_Memory(p) { if (p) delete(p); p = NULL; }

class CONTROL_EXPORT CFontFactory
{
public:
	CFontFactory();
	virtual ~CFontFactory();
	WCHAR* GetExistFontName(WCHAR *pFontName);
private:
	/* 私有变量 */
	WCHAR	*m_pFontnName;
	static	BYTE m_bIsEnumFont;	// 让字体枚举，只执行一次
	static	vector<CString> m_vFontName;	// 系统所有字体的名称

	/* 私有函数 */
	void GetEnumSystemFont();	/** 枚举系统所有字体 **/
	//lint -sem(CFontFactory::ReleaseFontName, cleanup)
	//lint -sem(CFontFactory::ReleaseFontName, initializer)
	inline void ReleaseFontName(WCHAR *p)
	{
		if(p)
		{
			delete[] p;
			p = NULL;
		}
	}

	BOOL AssertFontIsExist(const WCHAR *pFontName);
	static BOOL CALLBACK AFX_EXPORT EnumFamScreenCallBackEx(ENUMLOGFONTEX *pelf,NEWTEXTMETRICEX *lpntm,
															int FontType,LPVOID pThis);	/*回调函数，枚举系统已经安装的字体*/ 
};

extern CONTROL_EXPORT CFontFactory gFontFactory;	// 全局的字体工厂变量

#endif // !defined(AFX_FONTFACTORY_H__F6977B13_EA7B_4E4A_B4AC_C855002DC638__INCLUDED_)
