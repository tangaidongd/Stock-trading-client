#ifndef _GETMYCOMMANDLINE_H_
#define _GETMYCOMMANDLINE_H_

#include <vector>
#include <string>
#include <windows.h>
#include <stdio.h>

#define ArraySize(Array) (sizeof(Array)/sizeof(Array[0]))

#ifndef MyStringType

#ifdef UNICODE
#define MyStringType std::wstring
#else
#define MyStringType std::string
#endif	// UNICODE

#endif //!MyStringType

// ȥ�����߿ո�
inline char *trim(char *pszStr)
{
	int iLen = strlen(pszStr);
	
	int i = 0;
	// ȥ�����Ŀո�
	int iNewLen = 0;
	for(i=iLen-1; i>=0; --i)
	{
		char c = pszStr[i];
		if (c != ' ')
		{
			iNewLen = i+1;
			break;
		}
	}
	pszStr[iNewLen] = '\0';

	int iCur = 0;
	for(i=0; i<iNewLen; ++i)
	{
		char c = pszStr[i];
		if (c != ' ')
		{
			iCur = i;
			break;
		}
	}
	if (iCur > 0)
	{
		strcpy(pszStr, pszStr+iCur);
	}
	return pszStr;
}
// ȥ�����߿ո�
inline wchar_t *trim(wchar_t *pszStr)
{
	int iLen = wcslen(pszStr);

	int i = 0;
	// ȥ�����Ŀո�
	int iNewLen = 0;
	for(i=iLen-1; i>=0; --i)
	{
		char c = pszStr[i];
		if (c != ' ')
		{
			iNewLen = i+1;
			break;
		}
	}
	pszStr[iNewLen] = '\0';

	int iCur = 0;
	for(i=0; i<iNewLen; ++i)
	{
		char c = pszStr[i];
		if (c != ' ')
		{
			iCur = i;
			break;
		}
	}
	if (iCur > 0)
	{
		wcscpy(pszStr, pszStr+iCur);
	}
	return pszStr;
}
inline bool GetMyCommandLine(const TCHAR * pStr, int index, TCHAR *pszDest, int iMaxLen)
{
	if (pStr == NULL)
		return false;

	const int iMaxDestLen = iMaxLen;
	int iCur = 0;
	const int iLen = lstrlen(pStr);
	int iCount = 0;
	for(; iCur<iLen;)
	{
		// ȥ��ǰ��Ŀո�
		while(iCur<iLen)
		{
			if ( *(pStr+iCur) == TEXT(' '))
			{
				++iCur;
				continue;
			}
			break;
		}
		if (iCur >= iLen)
		{
			break;
		}
		

		TCHAR ch = *(pStr+iCur);
		TCHAR cEnd = TEXT(' ');
		if (ch == TEXT('\"'))
		{// �������˫����, ���˫������Ϊ������
			cEnd = TEXT('\"');
			++iCur;
		}

		if (iCur >= iLen)
		{
			break;
		}
		// ���ҽ�����
		int iStrCur = 0;
		while(iCur<iLen)
		{
			TCHAR ch = *(pStr+iCur);
			++iCur;
			if ( ch != cEnd)
			{
				if (iStrCur < iMaxDestLen-1)
				{
					if (index == iCount)
					{
						pszDest[iStrCur++] = ch;
					}
				}
				continue;
			}
			break;
		}
		if (index == iCount)
		{
			pszDest[iStrCur] = TEXT('\0');
			trim(pszDest);
			return true;
		}
		if (iCount > index)
		{
			break;
		}

		++iCount;
	}
	return false;
}

inline int GetMyCommandLineCount(const TCHAR * pStr)
{
	if (pStr == NULL)
		return 0;

	int iCur = 0;
	const int iLen = lstrlen(pStr);
	int iCount = 0;
	for(; iCur<iLen; )
	{
		// ȥ��ǰ��Ŀո�
		while(iCur<iLen)
		{
			if ( *(pStr+iCur) == TEXT(' '))
			{
				++iCur;
				continue;
			}
			break;
		}
		if (iCur >= iLen)
		{
			break;
		}

		TCHAR ch = *(pStr+iCur);
		TCHAR cEnd = TEXT(' ');
		if (ch == TEXT('\"'))
		{// �������˫����, ���˫������Ϊ������
			cEnd = TEXT('\"');
			++iCur;
		}
		
		if (iCur >= iLen)
		{
			break;
		}
		// ���ҽ�����
		while(iCur<iLen)
		{
			TCHAR ch = *(pStr+iCur);
			++iCur;
			if ( ch != cEnd)
			{
				continue;
			}
			break;
		}

		++iCount;
	}
	return iCount;
}

// �����������Ȳ��ܴ���127, ���򽫺���
typedef std::vector<MyStringType>	StringArray;
inline void GetMyCommandLine(const TCHAR * pStr, StringArray &strList)
{
	strList.clear();
	MyStringType strTmp;
	TCHAR szTmp[256];
	const int iMaxDestLen = ArraySize(szTmp);
	if (pStr == NULL)
		return ;

	int iCur = 0;
	const int iLen = lstrlen(pStr);
	for(; iCur<iLen; )
	{
		// ȥ��ǰ��Ŀո�
		while(iCur<iLen)
		{
			if ( *(pStr+iCur) == TEXT(' '))
			{
				++iCur;
				continue;
			}
			break;
		}
		if (iCur >= iLen)
		{
			break;
		}

		TCHAR ch = *(pStr+iCur);
		TCHAR cEnd = TEXT(' ');
		if (ch == TEXT('\"'))
		{// �������˫����, ���˫������Ϊ������
			cEnd = TEXT('\"');
			++iCur;
		}

		if (iCur >= iLen)
		{
			break;
		}
		// ���ҽ�����
		int iStrCur = 0;
		while(iCur<iLen)
		{
			TCHAR ch = *(pStr+iCur);
			++iCur;
			if ( ch != cEnd)
			{
				if (iStrCur < iMaxDestLen -1)
				{
					szTmp[iStrCur++] = ch;
				}				
				continue;
			}
			break;
		}
		szTmp[iStrCur] = TEXT('\0');
		trim(szTmp);
		strTmp = szTmp;
		strList.push_back(strTmp);
	}
}

#endif