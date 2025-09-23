#ifndef _LOG_FUNCTIME_H_
#define _LOG_FUNCTIME_H_
#include <atlstr.h>
#include "typedef.h"

class AFX_EXT_CLASS CLogFunctionTime
{
public:
	CLogFunctionTime(const CString& StrMsg);
	~CLogFunctionTime();
		
	void    Trace();
private:	
	bool32	m_bTraced;
	CString	m_StrMsg;
	DWORD	m_dwTimeBegin;
	DWORD   m_dwTimeEnd;
};

#endif