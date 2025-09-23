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

#define DEFAULT_FONT		L"����"
#define Release_Memory(p) { if (p) delete(p); p = NULL; }

class CONTROL_EXPORT CFontFactory
{
public:
	CFontFactory();
	virtual ~CFontFactory();
	WCHAR* GetExistFontName(WCHAR *pFontName);
private:
	/* ˽�б��� */
	WCHAR	*m_pFontnName;
	static	BYTE m_bIsEnumFont;	// ������ö�٣�ִֻ��һ��
	static	vector<CString> m_vFontName;	// ϵͳ�������������

	/* ˽�к��� */
	void GetEnumSystemFont();	/** ö��ϵͳ�������� **/
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
															int FontType,LPVOID pThis);	/*�ص�������ö��ϵͳ�Ѿ���װ������*/ 
};

extern CONTROL_EXPORT CFontFactory gFontFactory;	// ȫ�ֵ����幤������

#endif // !defined(AFX_FONTFACTORY_H__F6977B13_EA7B_4E4A_B4AC_C855002DC638__INCLUDED_)
