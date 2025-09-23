#include "StdAfx.h"
#include "LandMineStruct.h"
#include "MerchManager.h"
#include "sharestructnews.h"
#include "GmtTime.h"

//

int32 CStringToInt(TCHAR* pStrNum)
{
	if ( NULL == pStrNum )
	{
		return -1;
	}
	
	for ( int i = 0; i < (int)wcslen(pStrNum); i++ )
	{
		if ( !iswdigit(pStrNum[i]) )
		{
			return -1;
		}
	}
	
	//	
	USES_CONVERSION;
	char* pStrNumTmp = W2A((CString)pStrNum);
	
	int iReval = atoi(pStrNumTmp);
	
	return iReval;
}

int32 CStringToInt(const CString& StrNum)
{
	CString StrNumTmp = StrNum;
	
	TCHAR* pStrNum = StrNumTmp.LockBuffer();
	
	int32 iReval = CStringToInt(pStrNum);
	
	StrNumTmp.UnlockBuffer();
	
	return iReval;
}


bool32 BeValidSpecialName(TCHAR* pStrSpecialName)
{
	if ( 0 == wcscmp(pStrSpecialName, KStrKeyInfoIndex) )
	{
		return true;
	}
	
	if ( 0 == wcscmp(pStrSpecialName, KStrKeyBeginPos) )
	{
		return true;
	}
	
	if ( 0 == wcscmp(pStrSpecialName, KStrKeyTotalCount) )
	{
		return true;
	}
	
	if ( 0 == wcscmp(pStrSpecialName, KStrKeyMarket) )
	{
		return true;
	}
	
	if ( 0 == wcscmp(pStrSpecialName, KStrKeyCode) )
	{
		return true;
	}
	
	return false;
}

bool32 BeValidNormalName(TCHAR* pStrSpecialName)
{
	if ( 0 == wcscmp(pStrSpecialName, KStrKeyInfoIndex) )
	{
		return true;
	}
	
	if ( 0 == wcscmp(pStrSpecialName, KStrKeyDataType) )
	{
		return true;
	}
	
	if ( 0 == wcscmp(pStrSpecialName, KStrKeyInfoType) )
	{
		return true;
	}
	
	if ( 0 == wcscmp(pStrSpecialName, KStrKeyChildDataType) )
	{
		return true;
	}
	
	if ( 0 == wcscmp(pStrSpecialName, KStrKeyContent) )
	{
		return true;
	}
	
	if ( 0 == wcscmp(pStrSpecialName, KStrKeyUpdateTime) )
	{
		return true;
	}
	
	if ( 0 == wcscmp(pStrSpecialName, KStrKeyCrc32) )
	{
		return true;
	}

	if ( 0 == wcscmp(pStrSpecialName, KStrKeyAuthor) )
	{
		return true;
	}

	if ( 0 == wcscmp(pStrSpecialName, KStrKeyCategory) )
	{
		return true;
	}

	if ( 0 == wcscmp(pStrSpecialName, KStrKeyConLen) )
	{
		return true;
	}

	return false;
}

// 
bool32 SetLandeMineTitle(const CString& StrKey, const CString& StrValue, T_LandMineTitle& stLandMineTitle)
{
	if ( 0 == wcscmp(StrKey, KStrKeyInfoIndex) )
	{
		// 设置 InfoIndex
		int32 iValue = CStringToInt(StrValue);
		if ( -1 == iValue )
		{
			ASSERT(0);
			return false;
		}
		
		stLandMineTitle.m_iInfoIndex = iValue;
		return true;
	}
	
	if ( 0 == wcscmp(StrKey, KStrKeyContent) )
	{
		// 设置 Content
		stLandMineTitle.m_StrText = StrValue;
		
		return true;
	}
	
	if ( 0 == wcscmp(StrKey, KStrKeyUpdateTime) )
	{
		// 设置 UpdateTime
		stLandMineTitle.m_StrUpdateTime = StrValue;
		
		return true;
	}
	
	if ( 0 == wcscmp(StrKey, KStrKeyCrc32) )
	{
		// 设置 CRC32
		stLandMineTitle.m_StrCrc32 = StrValue;
		
		return true;
	}

	if ( 0 == wcscmp(StrKey, KStrKeyConLen) )
	{
		// 设置 conlen
		stLandMineTitle.m_iConLen = CStringToInt(StrValue);

		return true;
	}
		
	return false;
}

// 
bool32 SetLandeMineContent(const CString& StrKey, const CString& StrValue, T_LandMineContent& stLandMineContent)
{
	if ( 0 == wcscmp(StrKey, KStrKeyInfoIndex) )
	{
		// 设置 InfoIndex
		int32 iValue = CStringToInt(StrValue);
		if ( -1 == iValue )
		{
			ASSERT(0);
			return false;
		}
		
		stLandMineContent.m_iInfoIndex = iValue;
		return true;
	}
	
	if ( 0 == wcscmp(StrKey, KStrKeyContent) )
	{
		// 设置 Content
		stLandMineContent.m_StrText = StrValue;
		
		return true;
	}
	
	if ( 0 == wcscmp(StrKey, KStrKeyUpdateTime) )
	{
		// 设置 UpdateTime
		stLandMineContent.m_StrUpdateTime = StrValue;
		
		return true;
	}
	
	if ( 0 == wcscmp(StrKey, KStrKeyCrc32) )
	{
		// 设置 CRC32
		stLandMineContent.m_StrCrc32 = StrValue;
		
		return true;
	}
	
	if ( 0 == wcscmp(StrKey, KStrKeyConLen) )
	{
		// 设置 conlen
		stLandMineContent.m_iConLen = CStringToInt(StrValue);
		return true;
	}
	
	return false;
}

bool32 ParaseLandMineString(IN TCHAR* pStrLandMine, OUT int32& iMarketID, OUT CString& StrMerchCode, OUT vector<T_LandMineTitle>& aTitles, OUT T_LandMineContent& stContent, OUT CString& StrErrMsg)
{	
	aTitles.clear();
	stContent.Clear();
	iMarketID = -1;
	StrMerchCode.Empty();
	StrErrMsg.Empty();

	//
	int32 iStrLen = wcslen(pStrLandMine);

	//
	if ( iStrLen <= 0  )
	{
		StrErrMsg = L"传入字符串长度错误";
		return false;
	}

	// 解析头部的三个字段: 特殊字段数, 行数, 列数
	int32 iSpecial = 0;
	int32 iRows	 = 0;
	int32 iCols	 = 0;
	
	int32 iBeginPos	   = 0;
	int32 iProFeildCount = 0;
	
	// 特殊字段列头对应的索引
	map<int32, CString> aMapTableSpecial;
	
	// 普通字段索引
	map<int32, CString> aMapTableNormal;

	// 特殊字段中描述的 infoindex
	int32 iInfoIndexSpecial = -1;

	TCHAR* pCur;
	for (pCur = pStrLandMine; (pCur - pStrLandMine)< iStrLen && iProFeildCount < KiProFiledCounts; )
	{
		TCHAR StrTmp[1024] = {0};

		// 找到下一个分隔符
		TCHAR* pNext = wcschr(pCur, KcSeprator);
		if ( NULL == pNext )
		{
			ASSERT(0);
			StrErrMsg.Format(L"在当前字符串 %s 中找不到分隔符", pCur);
			return false;
		}

		// 拷贝两个分隔符中间的字符串
		int32 iLen = pNext - pCur;
		wcsncpy(StrTmp, pCur, iLen);
		StrTmp[iLen] = '\0';

		// 得到数字
		int32 iNum = CStringToInt(StrTmp);
		if ( -1 == iNum )
		{
			ASSERT(0);
			StrErrMsg.Format(L"字符串 %s 应该是纯数字字符串", StrTmp);
			return false;
		}

		// 赋值
		if ( 0 == iProFeildCount )
		{
			iSpecial = iNum;
		}
		else if ( 1 == iProFeildCount )
		{
			iRows = iNum;
		}
		else if ( 2 == iProFeildCount )
		{
			iCols = iNum;
		}

		//
		++iProFeildCount;
		pCur = pNext + 1;
	}
	
	// 解析协议体
	
	// 解析特殊字段名
	int32 i;
	for ( i = 0; i < iSpecial; i++ )
	{
		TCHAR StrTmp[1024] = {0};
		TCHAR* pNext = wcschr(pCur, KcSeprator);
		
		if ( NULL == pNext )
		{
			ASSERT(0);
			StrErrMsg.Format(L"解析特殊字段名: 在当前字符串 %s 中找不到分隔符", pCur);
			return false;
		}
		
		// 解析出一个字段名称
		int32 iLen = pNext - pCur;
		wcsncpy(StrTmp, pCur, iLen);
		StrTmp[iLen] = '\0';
		
		// 判断是否合法
		if ( !BeValidSpecialName(StrTmp) )
		{
			ASSERT(0);
			StrErrMsg.Format(L"非法的特殊字段名: %s", StrTmp);
			return false;
		}

		// 保存索引值
		aMapTableSpecial[i] = StrTmp;
		
		//
		pCur = pNext + 1;
	}
	
	// 解析普通的字段名
	for ( i = 0; i < iCols; i++ )
	{
		TCHAR StrTmp[1024] = {0};
		TCHAR* pNext = wcschr(pCur, KcSeprator);
		
		if ( NULL == pNext )
		{
			ASSERT(0);
			StrErrMsg.Format(L"解析普通字段名: 在当前字符串 %s 中找不到分隔符", pCur);
			return false;
		}
		
		// 解析出一个字段名称
		int32 iLen = pNext - pCur;
		wcsncpy(StrTmp, pCur, iLen);
		StrTmp[iLen] = '\0';
		
		// 判断是否合法
		if ( !BeValidNormalName(StrTmp) )
		{
			ASSERT(0);
			StrErrMsg.Format(L"非法的普通字段名: %s", StrTmp);
			return false;
		}
		
		// 保存索引值
		aMapTableNormal[i] = StrTmp;
		
		//
		pCur = pNext + 1;
	}
	
	// 解析特殊字段的值
	for ( i = 0; i < iSpecial; i++ )
	{
		TCHAR StrTmp[1024] = {0};
		TCHAR* pNext = wcschr(pCur, KcSeprator);
		
		if ( NULL == pNext )
		{
			ASSERT(0);
			StrErrMsg.Format(L"解析特殊字段值: 在当前字符串 %s 中找不到分隔符", pCur);
			return false;
		}
		
		// 解析出一个字段名称
		int32 iLen = pNext - pCur;
		wcsncpy(StrTmp, pCur, iLen);
		StrTmp[iLen] = '\0';
		
		// 设置特殊字段的值
		map<int32, CString>::iterator itFind = aMapTableSpecial.find(i);
		
		if ( aMapTableSpecial.end() != itFind )
		{
			CString StrKey = itFind->second;

			// Index
			if ( 0 == wcscmp(StrKey, KStrKeyInfoIndex) )
			{
				// 设置 InfoIndex
				int32 iValue = CStringToInt(StrTmp);
				if ( -1 == iValue )
				{
					ASSERT(0);
					StrErrMsg = L"特殊字段InfoIndex 解析错误";
					return false;
				}
				else
				{
					iInfoIndexSpecial = iValue;
				}
			}

			// Market
			if ( 0 == wcscmp(StrKey, KStrKeyMarket) )
			{			
				int32 iValue = CStringToInt(StrTmp);
				if ( -1 == iValue )
				{
					ASSERT(0);					
					StrErrMsg = L"市场号解析错误";
					return false;
				}
				else
				{
					iMarketID = iValue;			
				}
				
			}
			
			// Code
			if ( 0 == wcscmp(StrKey, KStrKeyCode) )
			{				
				StrMerchCode = StrTmp;
			}
		}
		
		//
		pCur = pNext + 1;
	}
		
	// 解析普通字段的节点值
	if ( EITLandmine == iInfoIndexSpecial )
	{		
		int32 iRowDealed = 0;
	
		for ( i = 0; i < iRows; i++ )
		{
			// 这一行的值
			T_LandMineTitle stTitle;

			// 遍历每一行
			for ( int32 j = 0; j < iCols; j++ )
			{
				// 遍历每一列
				TCHAR* pNext = wcschr(pCur, KcSeprator);
				
				if ( NULL == pNext )
				{
					ASSERT(0);
					StrErrMsg.Format(L"解析普通字段值: 在当前字符串 %s 中找不到分隔符, 表格第 %d 行, %d 列", pCur, i, j);
					return false;
				}
				
				int32 iLen = pNext - pCur;
				
				TCHAR* pStrTmp = new TCHAR[iLen + 10];
				wcsncpy(pStrTmp, pCur, iLen);
				pStrTmp[iLen] = '\0';

				// 设置字段值
				{
					map<int32, CString>::iterator itFind = aMapTableNormal.find(j);

					if ( aMapTableNormal.end() != itFind )
					{
						// 设置标题值.
						CString StrKey = itFind->second;
						SetLandeMineTitle(StrKey, pStrTmp, stTitle);
					}
					else
					{
						ASSERT(0);
					}
				}

				//
				DEL_ARRAY(pStrTmp);
				
				// 
				pCur = pNext + 1;
			}
			
			// 保存
			aTitles.push_back(stTitle);	
		}
	}
	else
	{
		// 当前的赋值
		ASSERT(iRows == 1);

		for ( i = 0; i < iRows; i++ )
		{
			// 遍历每一行
			for ( int32 j = 0; j < iCols; j++ )
			{
				// 遍历每一列
				TCHAR* pNext = wcschr(pCur, KcSeprator);
				
				if ( NULL == pNext )
				{
					ASSERT(0);
					StrErrMsg.Format(L"解析普通字段值: 在当前字符串 %s 中找不到分隔符, 表格第 %d 行, %d 列", pCur, i, j);
					return false;
				}
				
				int32 iLen = pNext - pCur;
				
				TCHAR* pStrTmp = new TCHAR[iLen + 10];
				wcsncpy(pStrTmp, pCur, iLen);
				pStrTmp[iLen] = '\0';
				
				// 设置字段值
				{
					map<int32, CString>::iterator it = aMapTableNormal.find(j);
					if ( aMapTableNormal.end() != it )
					{
						CString StrKey = it->second;
						SetLandeMineContent(StrKey, pStrTmp, stContent);
					}
				}
								
				//
				DEL_ARRAY(pStrTmp);
				
				// 
				pCur = pNext + 1;
			}						
		}
	}

	return true;
}

CString GmtTime2LandMineString(const CGmtTime& Time)
{
	CString StrTime;

	StrTime.Format(L"%04d-%02d-%02d %02d:%02d:%02d", Time.GetYear(), Time.GetMonth(), Time.GetDay(), Time.GetHour() + 8, Time.GetMinute(), Time.GetSecond());
	return StrTime;
}

bool32 LandMineString2GmtTime(const CString& StrTime, OUT CGmtTime& Time)
{
	// 2011-11-11 11:11:11
	if ( StrTime.IsEmpty() )
	{
		ASSERT(0);
		return false;
	}

	// 年
	int32 iPosBegin = 0;
	int32 iPosEnd	= StrTime.Find('-', iPosBegin);
	if ( -1 == iPosEnd )
	{
		ASSERT(0);
		return false;
	}
	
	CString StrYear = StrTime.Mid(iPosBegin, iPosEnd - iPosBegin);

	// 月
	iPosBegin = iPosEnd + 1;
	iPosEnd   = StrTime.Find('-', iPosBegin);
	if ( -1 == iPosEnd )
	{
		ASSERT(0);
		return false;
	}

	CString StrMonth = StrTime.Mid(iPosBegin, iPosEnd - iPosBegin);
	
	// 日
	iPosBegin = iPosEnd + 1;
	iPosEnd   = StrTime.Find(' ', iPosBegin);
	if ( -1 == iPosEnd )
	{
		ASSERT(0);
		return false;
	}
	
	CString StrDay = StrTime.Mid(iPosBegin, iPosEnd - iPosBegin);

	// 时
	iPosBegin = iPosEnd + 1;
	iPosEnd   = StrTime.Find(':', iPosBegin);
	if ( -1 == iPosEnd )
	{
		ASSERT(0);
		return false;
	}
	
	CString StrHour = StrTime.Mid(iPosBegin, iPosEnd - iPosBegin);

	// 分
	iPosBegin = iPosEnd + 1;
	iPosEnd   = StrTime.Find(':', iPosBegin);
	if ( -1 == iPosEnd )
	{
		ASSERT(0);
		return false;
	}
	
	CString StrMinute = StrTime.Mid(iPosBegin, iPosEnd - iPosBegin);

	// 秒
	CString StrSecond = StrTime.Mid(iPosEnd + 1, StrTime.GetLength() - iPosEnd);

	// 	
	int32 iYear	  = CStringToInt(StrYear);
	if ( iYear < 1970 )
	{
		ASSERT(0);
		return false;
	}

	int32 iMonth  = CStringToInt(StrMonth);
	if ( iMonth <= 0 || iMonth > 12 )
	{
		ASSERT(0);
		return false;
	}

	int32 iDay    = CStringToInt(StrDay);
	if ( iDay < 28 || iDay > 31 )
	{
		ASSERT(0);
		return false;
	}

	int32 iHour   = CStringToInt(StrHour);
	if ( iHour < 0 || iHour > 23 )
	{
		ASSERT(0);
		return false;
	}

	int32 iMinute = CStringToInt(StrMinute);
	if ( iMinute < 0 || iMinute > 59 )
	{
		ASSERT(0);
		return false;
	}

	int32 iSecond = CStringToInt(StrSecond);
	if ( iSecond < 0 || iSecond > 59 )
	{
		ASSERT(0);
		return false;
	}

	Time = CGmtTime(iYear, iMonth, iDay, iHour, iMinute, iSecond);

	return true;
}