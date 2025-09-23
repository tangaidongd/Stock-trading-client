#ifndef _PIN_YIN_H_
#define _PIN_YIN_H_

#define MAX_LEVEL2  16

struct T_PyObject
{
	CString			m_StrSectionName;
	CString			m_StrValue;

public:
	T_PyObject(const CString &StrSectionName, const CString &StrValue)
	{
		m_StrSectionName	= StrSectionName;
		m_StrValue			= StrValue;
	};
};

AFX_EXT_API CString	GetPyValue(const CString &StrSectionName);
AFX_EXT_API bool _is_alpha(char c);
AFX_EXT_API void InitPy();
AFX_EXT_API bool MatchPy(char *name,char *key,int keys);
AFX_EXT_API bool MatchPy(const CString &StrName, const CString &StrKey);
			
#endif
