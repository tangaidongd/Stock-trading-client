
#include "StdAfx.h"
#include "DeCodeIndex.h"
#include "sharestructnews.h"
#include "ShareFun.h"
#include "coding.h"
#include <map>

using namespace std;
//
static int32 CStringToInt(TCHAR* pStrNum)
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
	std::string StrNumA;
	Unicode2MultiChar(CP_ACP, pStrNum, StrNumA);
	int iReval = atoi(StrNumA.c_str());
	
	return iReval;
}

static int32 CStringToInt(const CString& StrNum)
{
	CString StrNumTmp = StrNum;
	
	TCHAR* pStrNum = StrNumTmp.LockBuffer();
	
	int32 iReval = CStringToInt(pStrNum);
	
	StrNumTmp.UnlockBuffer();
	
	return iReval;
}

bool32 GetIndexCode(IN TCHAR* pStrSrc, OUT CString& StrCode, OUT CString& StrContent)
{
	CString StrErrMsg = L"";
	int32 iStrLen = wcslen(pStrSrc);

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
	
	//int32 iBeginPos	   = 0;
	int32 iProFeildCount = 0;
	
	TCHAR* pCur;
	for ( pCur = pStrSrc; (pCur - pStrSrc)< iStrLen && iProFeildCount < KiProFiledCounts; )
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
// 		if ( !BeValidSpecialName(StrTmp) )
// 		{
// 			ASSERT(0);
// 			StrErrMsg.Format(L"非法的特殊字段名: %s", StrTmp);
// 			return false;
// 		}
		
		// 保存索引值
	
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
			StrErrMsg.Format(L"解析普通字段名: 在当前字符串 %s 中找不到分隔符",
				pCur);
			return false;
		}
		
		// 解析出一个字段名称
		int32 iLen = pNext - pCur;
		wcsncpy(StrTmp, pCur, iLen);
		StrTmp[iLen] = '\0';
		
		// 判断是否合法
// 		if ( !BeValidNormalName(StrTmp) )
// 		{
// 			ASSERT(0);
// 			StrErrMsg.Format(L"非法的普通字段名: %s", StrTmp);
// 			return false;
// 		}
		
		// 保存索引值
		
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
		TRACE(L"特殊字段值: %s", StrTmp);
		
		if ( i == (iSpecial - 1) )
		{
			StrCode = StrTmp;
		}

		//
		pCur = pNext + 1;
	}
		
	// 解析普通字段的节点值
	ASSERT(1 == iRows);

	//
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
			if ( 4 == j )
			{
				StrContent = CString(pStrTmp);
			}
			
			//
			DEL_ARRAY(pStrTmp);
			
			// 
			pCur = pNext + 1;
		}						
	}
	
	return true;
}


bool32 GetTitleList(IN TCHAR* pStrSrc, OUT listNewsInfo& listResult)
{
	listResult.RemoveAll();

	//
	CString StrErrMsg = L"";
	int32 iStrLen = wcslen(pStrSrc);

	//
	if ( iStrLen <= 0  )
	{
		StrErrMsg = L"传入字符串长度错误";
		return false;
	}

	// infoindex 特殊字段的值
	int32 iSpecialInfoIndexValue = -1;

	// infoindex 特殊字段的索引
	int32 iSpecialInfoIndexIndex = -1;

	// 解析头部的三个字段: 特殊字段数, 行数, 列数
	int32 iSpecial = 0;
	int32 iRows	 = 0;
	int32 iCols	 = 0;
	
	//int32 iBeginPos	   = 0;
	int32 iProFeildCount = 0;
	
	map<CString, int32> mapNoramlName;
	CStringArray aNormalValue;

	TCHAR* pCur;
	for ( pCur = pStrSrc; (pCur - pStrSrc)< iStrLen && iProFeildCount < KiProFiledCounts; )
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
		
		//
		if (0 == wcscmp(StrTmp, KStrKeyInfoIndex))
		{
			iSpecialInfoIndexIndex = i;
		}

		// 判断是否合法
// 		if ( !BeValidSpecialName(StrTmp) )
// 		{
// 			ASSERT(0);
// 			StrErrMsg.Format(L"非法的特殊字段名: %s", StrTmp);
// 			return false;
// 		}
		
		// 保存索引值
	
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
			StrErrMsg.Format(L"解析普通字段名: 在当前字符串 %s 中找不到分隔符",
				pCur);
			return false;
		}
		
		// 解析出一个字段名称
		int32 iLen = pNext - pCur;
		wcsncpy(StrTmp, pCur, iLen);
		StrTmp[iLen] = '\0';

		mapNoramlName[StrTmp] = i;
		
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
		TRACE(L"特殊字段值: %s", StrTmp);
		
		if (i == iSpecialInfoIndexIndex)
		{
			iSpecialInfoIndexValue = CStringToInt(StrTmp);
		}
		//
		pCur = pNext + 1;
	}
		
	//
	for ( i = 0; i < iRows; i++ )
	{
		T_NewsInfo stTitle;
		aNormalValue.RemoveAll();

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
			
			aNormalValue.Add(pStrTmp);
			
			//
			DEL_ARRAY(pStrTmp);
			
			// 
			pCur = pNext + 1;
		}	
		
		// 赋值

		// infoindex
		map<CString, int32>::iterator itFind = mapNoramlName.find(KStrKeyInfoIndex);
		if (itFind != mapNoramlName.end())
		{
			int32 iIdx = itFind->second;
			if (iIdx >= 0 && iIdx < aNormalValue.GetSize())
			{
				stTitle.m_iIndexID = CStringToInt(aNormalValue[iIdx]);
			}			
		}
		else
		{
			ASSERT(0);
		}

		// title
		itFind = mapNoramlName.find(KStrKeyTitle);
		if (itFind != mapNoramlName.end())
		{
			int32 iIdx = itFind->second;
			if (iIdx >= 0 && iIdx < aNormalValue.GetSize())
			{
				stTitle.m_StrContent = aNormalValue[iIdx];
			}			
		}
		else
		{
			itFind = mapNoramlName.find(KStrKeyContent);
			if (itFind != mapNoramlName.end())
			{
				int32 iIdx = itFind->second;
				if (iIdx >= 0 && iIdx < aNormalValue.GetSize())
				{
					stTitle.m_StrContent = aNormalValue[iIdx];
				}			
			}
			else
			{
				ASSERT(0);
			}
		}

		// updatetime
		itFind = mapNoramlName.find(KStrKeyUpdateTime);
		if (itFind != mapNoramlName.end())
		{
			int32 iIdx = itFind->second;
			if (iIdx >= 0 && iIdx < aNormalValue.GetSize())
			{
				stTitle.m_StrTimeUpdate = aNormalValue[iIdx];
			}			
		}
		else
		{
			ASSERT(0);
		}

		//
		stTitle.m_iInfoIndex = iSpecialInfoIndexValue;
		if (iSpecialInfoIndexValue < 0)
		{
			ASSERT(0);
		}

		//
		listResult.Add(stTitle);
	}
	
	return true;
}


bool32 GetPushTitle(IN TCHAR* pStrSrc, OUT T_NewsInfo& NewsInfo)
{
	//
	NewsInfo.m_iIndexID = 0;
	NewsInfo.m_iInfoIndex = 0;
	NewsInfo.m_StrContent = L"";
	NewsInfo.m_StrTimeUpdate = L"";

	//
	CString StrErrMsg = L"";
	int32 iStrLen = wcslen(pStrSrc);

	//
	if ( iStrLen <= 0  )
	{
		StrErrMsg = L"传入字符串长度错误";
		return false;
	}

	// ParentIndex 的值
	//int32 iParentIndexValue = -1;

	// infoindex 特殊字段的索引
	//int32 iParentIndexIndex = -1;

	// 解析头部的三个字段: 特殊字段数, 行数, 列数
	int32 iSpecial = 0;
	int32 iRows	 = 0;
	int32 iCols	 = 0;
	
	//int32 iBeginPos	   = 0;
	int32 iProFeildCount = 0;
	
	map<CString, int32> mapNoramlName;
	CStringArray aNormalValue;

	TCHAR* pCur;
	for ( pCur = pStrSrc; (pCur - pStrSrc)< iStrLen && iProFeildCount < KiProFiledCounts; )
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
			StrErrMsg.Format(L"解析普通字段名: 在当前字符串 %s 中找不到分隔符",
				pCur);
			return false;
		}
		
		// 解析出一个字段名称
		int32 iLen = pNext - pCur;
		wcsncpy(StrTmp, pCur, iLen);
		StrTmp[iLen] = '\0';

		mapNoramlName[StrTmp] = i;
		
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
		TRACE(L"特殊字段值: %s", StrTmp);

		//
		pCur = pNext + 1;
	}
		
	//
	ASSERT(1 == iRows);

	//
	for ( i = 0; i < iRows; i++ )
	{
		aNormalValue.RemoveAll();

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
			
			aNormalValue.Add(pStrTmp);
			
			//
			DEL_ARRAY(pStrTmp);
			
			// 
			pCur = pNext + 1;
		}	
		
		// 赋值

		// infoindex
		map<CString, int32>::iterator itFind = mapNoramlName.find(KStrKeyInfoIndex);
		if (itFind != mapNoramlName.end())
		{
			int32 iIdx = itFind->second;
			if (iIdx >= 0 && iIdx < aNormalValue.GetSize())
			{
				NewsInfo.m_iIndexID = CStringToInt(aNormalValue[iIdx]);
			}			
		}
		else
		{
			ASSERT(0);
		}

		// title
		itFind = mapNoramlName.find(KStrKeyTitle);
		if (itFind != mapNoramlName.end())
		{
			int32 iIdx = itFind->second;
			if (iIdx >= 0 && iIdx < aNormalValue.GetSize())
			{
				NewsInfo.m_StrContent = aNormalValue[iIdx];
			}			
		}
		else
		{
			itFind = mapNoramlName.find(KStrKeyContent);
			if (itFind != mapNoramlName.end())
			{
				int32 iIdx = itFind->second;
				if (iIdx >= 0 && iIdx < aNormalValue.GetSize())
				{
					NewsInfo.m_StrContent = aNormalValue[iIdx];
				}			
			}
			else
			{
				ASSERT(0);
			}
		}

		// updatetime
		itFind = mapNoramlName.find(KStrKeyUpdateTime);
		if (itFind != mapNoramlName.end())
		{
			int32 iIdx = itFind->second;
			if (iIdx >= 0 && iIdx < aNormalValue.GetSize())
			{
				NewsInfo.m_StrTimeUpdate = aNormalValue[iIdx];
			}			
		}
		else
		{
			ASSERT(0);
		}

		// ParentIndex
		itFind = mapNoramlName.find(KStrKeyParentIndex);
		if (itFind != mapNoramlName.end())
		{
			int32 iIdx = itFind->second;
			if (iIdx >= 0 && iIdx < aNormalValue.GetSize())
			{
				NewsInfo.m_iInfoIndex = CStringToInt(aNormalValue[iIdx]);
			}			
		}
		else
		{
			ASSERT(0);
		}
	}
	
	return true;
}