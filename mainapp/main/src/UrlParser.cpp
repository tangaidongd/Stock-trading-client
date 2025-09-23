#include "StdAfx.h"
#include "UrlParser.h"
#include "UrlConfig.h"



TCHAR Protocol[][20] = 
{
	_T("alias://"),
	_T("ftp://"),
	_T("http://"),
	_T("https://"),
	_T("file:///"),
	_T("tencent://"),
};
const int ProtocolNum = sizeof(Protocol) / sizeof(Protocol[0]);

const CString UrlBlank			= L"about:blank";

UrlParser::UrlParser()
{
	m_bValid  = false;
	m_protocol = none;
	m_port = 80;
}


UrlParser::UrlParser(LPCTSTR url)
{
	m_bValid  = false;
	m_protocol = none;
	m_port = 80;
	Parser(url);
}

void UrlParser::Parser(LPCTSTR url)
{
	CString strUrl = url;
	if (strUrl.IsEmpty())
	{
		m_bValid = false;
		return;
	}
	else if( 0 == strUrl.CompareNoCase(UrlBlank))
	{
		m_bValid = false;
		return;
	}
	else
	{
		//strUrl.Replace(_T(" "), _T(""));	// 删除链接中的空格
	}



	// 解析protocol
	int i;
	for(i = 0; i < ProtocolNum; i++)
	{
		if(0 == strUrl.Find(Protocol[i]))
		{
			m_protocol = (E_Protocol)i;
			break;
		}
	}



	// 解析 port, hostname
	int nPos = wcslen(Protocol[m_protocol]);
	CString strHostname = strUrl.Mid(nPos);

	// 解析path
	int nPathBegin = strUrl.Find(_T('/'), nPos);
	int nPosQuestion = strUrl.Find(_T('?'), nPos);	// query
	int nPosSemicolon = strUrl.Find(_T(';'), nPos);	// parameters
	int nPosWell = strUrl.Find(_T('#'), nPos);	// fragment
	/** 保存3种分隔符的原来位置 */
	memset(symbol, sizeof(symbol) / sizeof(symbol[0]), 0);
	symbol[0][0] = nPosQuestion;
	symbol[0][1] = nPosSemicolon;
	symbol[0][2] = nPosWell;


	int nPosMin = GetMinPos(nPosQuestion, nPosSemicolon, nPosWell);

	//判断服务器地址
	if(nPathBegin != -1)
	{
		strHostname = strUrl.Mid(nPos,nPathBegin-nPos);
	}
	else
	{
		if (nPosMin != -1)
		{
			strHostname = strUrl.Mid(nPos,nPosMin-nPos);
		}
		else
		{
			strHostname = strUrl.Mid(nPos);
		}
	}
	nPos += strHostname.GetLength();
	

	if (m_protocol == alias)
	{
		CString strUrl = CUrlConfig::Instance().GetUrl(strHostname);
		Parser(strUrl);
		return;
	}
	else if (m_protocol != files)
	{
		/** 如果是非本地文件路径，要分拆服务器地址和端口*/
		int nPosPort = strHostname.ReverseFind(_T(':'));
		if(-1 != nPosPort)
		{
			CString strTemp = strHostname.Mid(nPosPort + 1);
			//if (!strTemp.IsEmpty())
			{
				m_port = _ttoi(strTemp);
			}
			strHostname = strHostname.Left(nPosPort);
		}
	}
	else 
	{
		/** 本地文件路径，盘符为服务器地址*/
	}

	SetHostname(strHostname);

	if(nPosMin != -1)
	{
		m_path = strUrl.Mid(nPos, nPosMin - nPos);
	}
	else
	{
		m_path = strUrl.Mid(nPos, strUrl.GetLength() - nPos);
	}
	nPos += m_path.GetLength();
	// 解析parameters 解析query 解析fragment
	int Len = strUrl.GetLength();
	int posArr[] = { nPosQuestion, nPosSemicolon, nPosWell, Len };
	SortArry( posArr, sizeof(posArr) / sizeof(posArr[0]) );
	for(i = 0; i < 3; i++)
	{
		if(nPosQuestion != -1 && nPosQuestion == posArr[i])
		{
			int nPosEnd = posArr[i + 1];
			if(nPosSemicolon == -1 && nPosWell == -1)
			{
				nPosEnd = strUrl.GetLength();
			}

			m_query = strUrl.Mid(1 + posArr[i], nPosEnd - posArr[i] - 1);
		}
		else if(nPosSemicolon != -1 && nPosSemicolon == posArr[i])
		{
			m_parameters = strUrl.Mid(1 + posArr[i], posArr[i + 1] - posArr[i] - 1);
		}
		else if(nPosWell != -1 && nPosWell == posArr[i])
		{
			m_fragment = strUrl.Mid(1 + posArr[i], posArr[i + 1] - posArr[i] - 1);
		}
	}
	/**  把排了序的三种符合顺序保存下来 **/
	symbol[1][0] = posArr[0];
	symbol[1][1] = posArr[1];
	symbol[1][2] = posArr[2];
}


void UrlParser::SetHostname(LPCTSTR hostname)
{
	CString strHostname = hostname;

	if (0 == strHostname.CompareNoCase(L"localpath"))
	{
		m_protocol = files;
	
		TCHAR szPath[MAX_PATH] = {0};
		GetModuleFileName(NULL, szPath, MAX_PATH);
		CString StrPath = szPath;
		StrPath.Replace('\\','/');
		int pos = StrPath.ReverseFind('/');
		if (pos > 0)
		{ 
			strHostname = StrPath.Left(pos);
		}
		else
		{
			strHostname = StrPath;
		}
	}

	m_strHostname = strHostname;

	m_bValid = true;
}

void UrlParser::SortArry(int arr[], int num)
{return ;
	for(int i= 1; i< num; i++)
	{  
		if(arr[i] < arr[i-1])
		{//若第i个元素大于i-1元素，直接插入。小于的话，移动有序表后插入  
			int j= i-1;   
			int x = arr[i];        //复制为哨兵，即存储待排序元素  
			arr[i] = arr[i-1];           //先后移一个元素  
			while(x < arr[j])
			{  //查找在有序表的插入位置  
				arr[j+1] = arr[j];  
				j--;         //元素后移  
			}  
			arr[j+1] = x;      //插入到正确位置  
		}
	} 
}

int UrlParser::GetMinPos(int x, int y, int z)
{
	int tempXY = min(x, y);
	if(tempXY < 0)
	{
		tempXY = max(x, y);
	}

	int temp = min(tempXY, z);
	if(temp < 0)
	{
		temp = max(tempXY, z);
	}

	return temp;
}

UrlParser::~UrlParser(void)
{
}

/*
	error 568: (Warning -- non-negative quantity is never less than zero)
*/
//lint --e{568}
CString UrlParser::GetUrl()
{
	if (!m_bValid)
	{
		return UrlBlank;
	}


	CString strUrl;
	if(m_protocol == none)
	{
		strUrl = _T("http://");
	}
	else
	{
		strUrl = Protocol[m_protocol];
	}

	strUrl += m_strHostname;
	if(m_port != 80)
	{
		CString strTemp;
		strTemp.Format(_T(":%d"), m_port);
		strUrl += strTemp;
	}

	if(!m_path.IsEmpty())
	{
		strUrl += m_path;
	}

	AddSymbol(strUrl);

	return strUrl;
}//	protocol :// hostname[:port]/path/[;parameters][?query]#fragment

void UrlParser::AddSymbol(CString& strUrl)
{
	// 符号顺序是 ?;# 在symbol[0]中
	strUrl += AddQuery();
	strUrl += AddParameters();
	strUrl += AddFragment();



	//if(symbol[1][0] == symbol[0][0])	// ?号
	//{
	//	strUrl += AddQuery();
	//}
	//else if(symbol[1][0] == symbol[0][1])	// ;号
	//{
	//	strUrl += AddParameters();
	//}
	//else if(symbol[1][0] == symbol[0][2])	// #号
	//{
	//	strUrl += AddFragment();
	//}
	////
	//if(symbol[1][1] == symbol[0][0])	// ?号
	//{
	//	strUrl += AddQuery();
	//}
	//else if(symbol[1][1] == symbol[0][1])	// ;号
	//{
	//	strUrl += AddParameters();
	//}
	//else if(symbol[1][1] == symbol[0][2])	// #号
	//{
	//	strUrl += AddFragment();
	//}
	////
	//if(symbol[1][2] == symbol[0][0])	// ?号
	//{
	//	strUrl += AddQuery();
	//}
	//else if(symbol[1][2] == symbol[0][1])	// ;号
	//{
	//	strUrl += AddParameters();
	//}
	//else if(symbol[1][2] == symbol[0][2])	// #号
	//{
	//	strUrl += AddFragment();
	//}

}

CString UrlParser::AddParameters()
{
	CString strTemp;
	if(!m_parameters.IsEmpty())
	{
		strTemp += _T(";");
		strTemp += m_parameters;
	}

	return strTemp;
}

CString UrlParser::AddQuery()
{
	CString strTemp;
	if(!m_query.IsEmpty())
	{
		strTemp += _T("?");
		strTemp += m_query;
	}

	return strTemp;
}

CString UrlParser::AddFragment()
{
	CString strTemp;
	if(!m_fragment.IsEmpty())
	{
		strTemp += _T("#");
		strTemp += m_fragment;
	}

	return strTemp;
}

void UrlParser::SetQueryValue(LPCTSTR key, LPCTSTR value )
{

	CString strNewKeyValue = key;
	strNewKeyValue += _T("=");
	strNewKeyValue += value;

	CString strOldValue = GetQueryValue(key);
	if (strOldValue.IsEmpty())
	{
		if (!m_query.IsEmpty())
		{
			m_query += _T("&");
		}
		m_query += strNewKeyValue;
	}
	else
	{
		CString stOldKeyValue = key;
		stOldKeyValue += _T("=");
		stOldKeyValue += strOldValue;
		m_query.Replace(stOldKeyValue,strNewKeyValue);
	}
}

CString UrlParser::GetQueryValue(LPCTSTR key)
{
	if (!m_query.IsEmpty())
	{
		int nPosStrat = 0;
		int nPosEnd = 0;
		while (true)
		{
			//先分解出每一个KEY-VALUE串
			CString strKeyValue;
			nPosEnd = m_query.Find('&',nPosStrat);
			if (-1 == nPosEnd)
			{
				if (0==nPosStrat)
				{
					strKeyValue = m_query;
				}
				else
				{
					strKeyValue = m_query.Mid(nPosStrat);
				}
			}
			else
			{
				strKeyValue = m_query.Mid(nPosStrat,nPosEnd-nPosStrat);
				nPosStrat = nPosEnd + 1;
			}

			//开始匹配KEY
			int nPos = strKeyValue.Find('=');
			if (-1 != nPos)
			{
				CString strKey = strKeyValue.Left(nPos);
				if (strKey == key)
				{
					//完美匹配上一个KEY，返回的VALUE并退出循环
					return strKeyValue.Mid(nPos+1);
				}
			}

			//整个串找完了，也没匹配上KEY，退出循环
			if (-1 == nPosEnd)
			{
				break;
			}
		}
	}

	return L"";
}


CString UrlParser::GetQuery()
{
	return m_query;
}

void UrlParser::SetQuery(LPCTSTR query,bool bAdd /*= false*/)
{
	if (query == NULL)
	{
		return;
	}

	if (bAdd)
	{
		if (!m_query.IsEmpty())
		{
			m_query.AppendChar('&');
		}
		m_query.Append(query);
	}
	else
	{
		m_query = query;
	}
}