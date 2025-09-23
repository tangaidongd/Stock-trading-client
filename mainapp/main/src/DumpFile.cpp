#include "StdAfx.h"
#include <stdio.h>
#include <windows.h>
#include "DbgHelp.h"
#include "DumpFile.h"
#include "ShareFun.h"

#pragma comment(lib, "Dbghelp.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

LONG WINAPI MyUnhandledFilter(struct _EXCEPTION_POINTERS *lpExceptionInfo)
{
	LONG ret = EXCEPTION_CONTINUE_SEARCH;//EXCEPTION_EXECUTE_HANDLER;
	
	TCHAR szFileName[64];
	SYSTEMTIME st;
	::GetLocalTime(&st);
	wsprintf(szFileName, TEXT("dump\\%04d-%02d-%02d-%02d-%02d-%02d-%02d-%02d.dmp"), st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds, rand()%100);
	
	HANDLE hFile = ::CreateFile(szFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
	if (hFile != INVALID_HANDLE_VALUE)
	{
		MINIDUMP_EXCEPTION_INFORMATION ExInfo;
		
		ExInfo.ThreadId = ::GetCurrentThreadId();
		ExInfo.ExceptionPointers = lpExceptionInfo;
		ExInfo.ClientPointers = false;
		
		// write the dump
		
		BOOL bOK = MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hFile, MiniDumpNormal, &ExInfo, NULL, NULL );
		
		if (bOK)
		{
			TRACE(_T("Create Dump File Success!\n"));
		}
		else
		{
			//printf("MiniDumpWriteDump Failed: %d\n", GetLastError());
			TRACE(_T("Create Dump File Success!\n"));
		}
		
		DEL_HANDLE(hFile);
	}
	else
	{
		//printf("Create File %s Failed %d\n", szFileName, GetLastError());
		TRACE(_T("Create File %s Failed %d\n"), szFileName, GetLastError());
	}
	return ret;
}