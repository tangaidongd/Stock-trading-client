#ifndef _LANDMINE_STRUCT_H_
#define _LANDMINE_STRUCT_H_

#include "InfoExport.h"



class CGmtTime;

// 信息地雷标题对应的内容
class DATAINFO_DLL_EXPORT CLandMineContent
{
public:
	CLandMineContent()
	{
		Clear();
	}

	void Clear()
	{
		m_iInfoIndex = 0;		
		m_iConLen	 = 0;
		m_StrText.Empty();
		m_StrCrc32.Empty();
		m_StrUpdateTime.Empty();
	}

	bool32 CLandMineContent::operator==(const CLandMineContent& stOther) const
	{
		if ( m_iInfoIndex != stOther.m_iInfoIndex )
		{
			return false;
		}

		if ( m_iConLen != stOther.m_iConLen )
		{
			return false;
		}

		if ( 0 != m_StrText.CompareNoCase(stOther.m_StrText) )
		{
			return false;
		}

		if ( 0 != m_StrCrc32.CompareNoCase(stOther.m_StrCrc32) )
		{
			return false;
		}

		if ( 0 != m_StrUpdateTime.CompareNoCase(stOther.m_StrUpdateTime) )
		{
			return false;
		}

		return true;
	}

	int32		m_iInfoIndex;				// 索引值	
	int32		m_iConLen;					// 长度
	CString		m_StrText;					// 内容
	CString		m_StrCrc32;					// 校验值
	CString		m_StrUpdateTime;			// 更新时间

};

// 信息地雷的标题
class DATAINFO_DLL_EXPORT CLandMineTitle
{
public:
	CLandMineTitle()
	{
		Clear();
	}

	void Clear()
	{
		m_iInfoIndex = 0;		
		m_iConLen	 = 0;
		m_StrText.Empty();
		m_StrCrc32.Empty();
		m_StrUpdateTime.Empty();

		m_stContent.Clear();
	}

	bool32 CLandMineTitle::operator==(const CLandMineTitle& stOther) const
	{
		if ( m_iInfoIndex != stOther.m_iInfoIndex )
		{
			return false;
		}

		if ( m_iConLen != stOther.m_iConLen )
		{
			return false;
		}

		if ( 0 != m_StrText.CompareNoCase(stOther.m_StrText) )
		{
			return false;
		}

		if ( 0 != m_StrCrc32.CompareNoCase(stOther.m_StrCrc32) )
		{
			return false;
		}

		if ( 0 != m_StrUpdateTime.CompareNoCase(stOther.m_StrUpdateTime) )
		{
			return false;
		}

		if ( 0 == (m_stContent == stOther.m_stContent) )
		{
			return false;
		}

		return true;
	}

	int32		m_iInfoIndex;				// 索引值	
	int32		m_iConLen;					// 长度
	CString		m_StrText;					// 内容	
	CString		m_StrCrc32;					// 校验值
	CString		m_StrUpdateTime;			// 更新时间

	CLandMineContent m_stContent;			// 标题对应的内容
};

// 信息地雷的数据结构, 同一天可能多个地雷, 目前时间的精确度只到天, 所以同天的地雷时间是完全相同的.
typedef multimap<CString, CLandMineTitle>  mapLandMine;

// 迭代器
typedef multimap<CString, CLandMineTitle>::iterator itLandMine;

// 迭代器
typedef multimap<CString, CLandMineTitle>::const_iterator KitLandMine;

// 查找返回范围
typedef pair<KitLandMine, KitLandMine> RangeLandMine;

// 解析信息地雷的回包字符串, 解出来的可能是一组标题, 也可能是某个标题的内容
DATAINFO_DLL_EXPORT bool32 ParaseLandMineString(IN TCHAR* pStrF10, OUT int32& iMarketID, OUT CString& StrMerchCode, OUT vector<CLandMineTitle>& aTitles, OUT CLandMineContent& stContent, OUT CString& StrErrMsg);

// 转换时间
DATAINFO_DLL_EXPORT CString GmtTime2LandMineString(const CGmtTime& Time);

DATAINFO_DLL_EXPORT bool32 LandMineString2GmtTime(const CString& StrTime, OUT CGmtTime& Time);

#endif // _LANDMINE_STRUCT_H_