#ifndef _DECODEINDEX_H_
#define _DECODEINDEX_H_
#include "typedef.h"
#include "CenterManagerStruct.h"



bool32 GetIndexCode(IN TCHAR* pStrSrc, OUT CString& StrCode, OUT CString& StrContent);

bool32 GetTitleList(IN TCHAR* pStrSrc, OUT listNewsInfo& listResult);

bool32 GetPushTitle(IN TCHAR* pStrSrc, OUT T_NewsInfo& NewsInfo);

#endif //_DECODEINDEX_H_
