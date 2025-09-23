#ifndef _LANDMINE_STRUCT_H_
#define _LANDMINE_STRUCT_H_

#include "InfoExport.h"



class CGmtTime;

// ��Ϣ���ױ����Ӧ������
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

	int32		m_iInfoIndex;				// ����ֵ	
	int32		m_iConLen;					// ����
	CString		m_StrText;					// ����
	CString		m_StrCrc32;					// У��ֵ
	CString		m_StrUpdateTime;			// ����ʱ��

};

// ��Ϣ���׵ı���
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

	int32		m_iInfoIndex;				// ����ֵ	
	int32		m_iConLen;					// ����
	CString		m_StrText;					// ����	
	CString		m_StrCrc32;					// У��ֵ
	CString		m_StrUpdateTime;			// ����ʱ��

	CLandMineContent m_stContent;			// �����Ӧ������
};

// ��Ϣ���׵����ݽṹ, ͬһ����ܶ������, Ŀǰʱ��ľ�ȷ��ֻ����, ����ͬ��ĵ���ʱ������ȫ��ͬ��.
typedef multimap<CString, CLandMineTitle>  mapLandMine;

// ������
typedef multimap<CString, CLandMineTitle>::iterator itLandMine;

// ������
typedef multimap<CString, CLandMineTitle>::const_iterator KitLandMine;

// ���ҷ��ط�Χ
typedef pair<KitLandMine, KitLandMine> RangeLandMine;

// ������Ϣ���׵Ļذ��ַ���, ������Ŀ�����һ�����, Ҳ������ĳ�����������
DATAINFO_DLL_EXPORT bool32 ParaseLandMineString(IN TCHAR* pStrF10, OUT int32& iMarketID, OUT CString& StrMerchCode, OUT vector<CLandMineTitle>& aTitles, OUT CLandMineContent& stContent, OUT CString& StrErrMsg);

// ת��ʱ��
DATAINFO_DLL_EXPORT CString GmtTime2LandMineString(const CGmtTime& Time);

DATAINFO_DLL_EXPORT bool32 LandMineString2GmtTime(const CString& StrTime, OUT CGmtTime& Time);

#endif // _LANDMINE_STRUCT_H_