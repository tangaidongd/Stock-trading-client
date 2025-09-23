#ifndef _XLTIMERTRACE_H_
#define _XLTIMERTRACE_H_

#include <mmsystem.h>

#ifdef DEBUG

// 只能小于60个空格，避免无穷计算
#define XLTRACE_ADDSPACE_STRING(StrSpace, iSpace)	\
				{ \
					ASSERT( iSpace <= 60 && iSpace >= 0 ); \
					for (int _i##iSpace = 0; _i##iSpace < iSpace; _i##iSpace += 2 ) \
						StrSpace += _T("  "); \
				}
#define XLTRACE_USEFILE
#ifdef XLTRACE_USEFILE

#include "XLTraceFile.h"

#define XLTRACE XLTraceFile::GetXLTraceFile().Trace
#define XLTRACE_FLUSH XLTraceFile::GetXLTraceFile().FlushXLTraceFile();
//end file trace
#else //normal

#define XLTRACE TRACE
#define XLTRACE_FLUSH

#endif //end XLTraceFile

#define XLTRACE_CREATE_ID(id)		  DWORD  _##id;

#define XLTRACE_TIME_START_ID(id)		XLTRACE_PERFORMANCE_START_ID(id)
#define XLTRACE_TIME_END_ID(id)			(timeGetTime() - _##id)

#define XLTRACE_PERFORMANCE_RESET_ID(id)		 _##id = timeGetTime();
#define XLTRACE_PERFORMANCE_START_ID(id)  XLTRACE_CREATE_ID(id) \
										  XLTRACE_PERFORMANCE_RESET_ID(id)
#define XLTRACE_PERFORMANCE_START_INFO_ID(id, StrSpace, className, funcName, StrYours) \
				XLTRACE_PERFORMANCE_START_ID(id) \
				XLTRACE(CString(StrSpace) + #className + _T(".") + #funcName + _T("(") + CString(StrYours) + _T("): START\r\n")); \
				XLTRACE_FLUSH

#define XLTRACE_PERFORMANCE_END_ID(id, StrSpace, className, funcName, StrYours)  \
				XLTRACE(CString(StrSpace) + #className + _T(".") + #funcName + _T("(") + CString(StrYours) + _T("): %d ms\r\n"), XLTRACE_TIME_END_ID(id) );\
				XLTRACE_FLUSH


#define XLTRACE_TIME_START  XLTRACE_TIME_START_ID(dwTimeStart_)
#define XLTRACE_TIME_END    XLTRACE_TIME_END_ID(dwTimeStart_)

#define XLTRACE_PERFORMANCE_START	XLTRACE_PERFORMANCE_START_ID(dwTimeStart2_)
#define XLTRACE_PERFORMANCE_RESET	XLTRACE_PERFORMANCE_RESET_ID(dwTimeStart2_)
#define XLTRACE_PERFORMANCE_START_INFO(StrSpace, className, funcName, StrYours) \
				XLTRACE_PERFORMANCE_START_INFO_ID(dwTimeStart2_, StrSpace, className, funcName, StrYours)

#define XLTRACE_PERFORMANCE_END(StrSpace, className, funcName, StrYours)  \
				XLTRACE_PERFORMANCE_END_ID(id, StrSpace, className, funcName, StrYours)

// 在函数内部使用，自动在退出时打印
class AFX_EXT_CLASS XLTraceAuto{
public:
	XLTraceAuto(LPCTSTR lpszSpace, LPCSTR lpszClassName, LPCSTR lpszFuncName, LPCTSTR lpszYours, BOOL bShowStartInfo = FALSE);
	~XLTraceAuto();
private:
	XLTRACE_CREATE_ID(m_dwId)
	const CString m_StrSpace;
	const CString m_StrClassName;
	const CString m_StrFuncName;
	const CString m_StrYours;
};

#define XLTRACE_PERFORMANCE_AUTO(StrSpace, className, funcName, StrYours)	    XLTraceAuto _##className##funcName##_1(StrSpace, #className, #funcName, StrYours);
#define XLTRACE_PERFORMANCE_AUTO_INFO(StrSpace, className, funcName, StrYours)	XLTraceAuto _##className##funcName##_2(StrSpace, #className, #funcName, StrYours, TRUE);
// end normal trace

class AFX_EXT_CLASS XLTraceSimpleAuto
{
public:
	XLTraceSimpleAuto(LPCTSTR ptszDesc)
		: m_StrDesc(ptszDesc)
	{
		m_dwTime = timeGetTime();
	}
	~XLTraceSimpleAuto()
	{
		DWORD dwTime2 = timeGetTime();
		TRACE(_T("%s: %d ms\r\n"), m_StrDesc, dwTime2-m_dwTime);
	}

private:
	CString		m_StrDesc;
	DWORD		m_dwTime;
};
#define XLTRACE_SIMPLE_AUTO(desc, obj)		XLTraceSimpleAuto	obj(desc)

#else  //_NDEBUG

#define XLTRACE_ADDSPACE_STRING(StrSpace, iSpace)

inline void XLTraceEmpty(LPCTSTR, ...){}
#define XLTRACE XLTraceEmpty

#define XLTRACE_TIME_START  
#define XLTRACE_TIME_END    

#define XLTRACE_PERFORMANCE_START  
#define XLTRACE_PERFORMANCE_RESET
#define XLTRACE_PERFORMANCE_START_INFO(StrSpace, className, funcName, StrYours) 

#define XLTRACE_PERFORMANCE_END(StrSpace, className, funcName, StrYours)

#define XLTRACE_PERFORMANCE_START_ID(id) 
#define XLTRACE_PERFORMANCE_RESET_ID(id)
#define XLTRACE_PERFORMANCE_START_INFO_ID(id, StrSpace, className, funcName, StrYours) 

#define XLTRACE_PERFORMANCE_END_ID(id, StrSpace, className, funcName, StrYours)


#define XLTRACE_PERFORMANCE_AUTO(StrSpace, className, funcName, StrYours)
#define XLTRACE_PERFORMANCE_AUTO_INFO(StrSpace, className, funcName, StrYours)

#define XLTRACE_SIMPLE_AUTO(desc, obj)

#endif //DEBUG
#endif