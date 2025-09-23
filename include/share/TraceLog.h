#ifndef TRACE_LOG_H
#define TRACE_LOG_H

#include "synch.h"

#ifndef _MYTRACE
#ifdef _TRACELOG
#define _MYTRACE CTraceLog::Ptr()->TraceLog
#else
#ifdef _DEBUG
#define _MYTRACE TRACE
#else
#define _MYTRACE
#endif // _DEBUG
#endif // _TRACELOG
#endif

#ifndef _TESTTRACE
#define _TESTTRACE CTraceLog::Ptr()->TraceLog
#endif

#ifndef _DLLTRACE
#define _DLLTRACE CTraceLog::DllLog
#endif	

//
class AFX_EXT_CLASS CTraceLog
{
public:
	//
	static CTraceLog* Ptr();
	void		Del();

	//
	void		TraceLog(LPCTSTR StrFormat, ...);

	//
	static void	DllLog(LPCTSTR StrFormat, ...);

	~CTraceLog();

private:
	CTraceLog();

	class CAssist
	{
	public:
		void	lock()		{ m_lock.lock();   }
		void	unlock()	{ m_lock.unlock(); }

	private:
		LockSingle m_lock;
	};

private:
	static CTraceLog*  m_pThis;
	static CAssist	   m_stLockThis;

	FILE*		m_pFile;	
	LockSingle	m_LockFile;

	static HWND	m_hWndTrace;
	static bool m_bLog;
};

#endif // TRACE_LOG_H