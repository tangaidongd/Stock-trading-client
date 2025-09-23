#ifndef _DECODEINDEX_H_
#define _DECODEINDEX_H_
#include "typedef.h"
#include "DataCenterExport.h"


// 新闻内容
typedef struct T_NewsInfo
{
public:
	int32		m_iInfoIndex;		// 资讯类型ID
	int32		m_iIndexID;			// 资讯id
	CString		m_StrContent;		// 标题内容
	CString		m_StrTimeUpdate;	// 更新时间

}T_NewsInfo;

typedef CArray<T_NewsInfo, T_NewsInfo&> listNewsInfo;

DATACENTER_DLL_EXPORT bool32 GetIndexCode(IN TCHAR* pStrSrc, OUT CString& StrCode, OUT CString& StrContent);

DATACENTER_DLL_EXPORT bool32 GetTitleList(IN TCHAR* pStrSrc, OUT listNewsInfo& listResult);

DATACENTER_DLL_EXPORT bool32 GetPushTitle(IN TCHAR* pStrSrc, OUT T_NewsInfo& NewsInfo);

#endif //_DECODEINDEX_H_
