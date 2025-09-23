#ifndef _DECODEINDEX_H_
#define _DECODEINDEX_H_
#include "typedef.h"
#include "DataCenterExport.h"


// ��������
typedef struct T_NewsInfo
{
public:
	int32		m_iInfoIndex;		// ��Ѷ����ID
	int32		m_iIndexID;			// ��Ѷid
	CString		m_StrContent;		// ��������
	CString		m_StrTimeUpdate;	// ����ʱ��

}T_NewsInfo;

typedef CArray<T_NewsInfo, T_NewsInfo&> listNewsInfo;

DATACENTER_DLL_EXPORT bool32 GetIndexCode(IN TCHAR* pStrSrc, OUT CString& StrCode, OUT CString& StrContent);

DATACENTER_DLL_EXPORT bool32 GetTitleList(IN TCHAR* pStrSrc, OUT listNewsInfo& listResult);

DATACENTER_DLL_EXPORT bool32 GetPushTitle(IN TCHAR* pStrSrc, OUT T_NewsInfo& NewsInfo);

#endif //_DECODEINDEX_H_
