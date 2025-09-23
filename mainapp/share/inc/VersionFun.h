// VersionFun.h: interface for the CVersionFun class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _VERSION_FUN_H_
#define _VERSION_FUN_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define FORCEUPDATED	0

class CBindStatusCB
{
public:
	virtual bool OnProgress(int iProcess,int iMax) = 0;
};

int HttpDownLoad ( TCHAR* StrHttp,TCHAR* StrLocal, CBindStatusCB* pCB );
//BOOL _tcheck_if_mkdir(TCHAR *dir_or_file);
CString GetFileVersion ( CString StrFile);
time_t GetFileLMTime( CString StrPath );

#endif //_VERSION_FUN_H_