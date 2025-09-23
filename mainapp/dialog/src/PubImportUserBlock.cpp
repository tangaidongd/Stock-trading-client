#include "stdafx.h"
#include "PubImportUserBlock.h"
#include <vector>
#include <vector>

using namespace std;


#include "excel.h"
using namespace excel;



#define  REG_TDX_APP_EXE_PATH  _T("AppExePath")
#define  REG_TDX_APP_PATH	   _T("InstallLocation")

#define  REG_TDX_RUN_TOOL		_T("Tdxw.exe")
#define  REG_DZH_RUN_TOOL		_T("dzhtool.exe")
#define  REG_THS_RUN_TOOL		_T("hexin.exe")

#define  REG_APP_EXE_PATH	   _T("DisplayIcon")
#define  REG_DISPLAY_NAME	   _T("DisplayName")
#define  REG_APP_PATH		   _T("Inno Setup: App Path")

#define  REG_KEY_ITEM	 _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall")



CPubImportUserBlockFun::CPubImportUserBlockFun()
{
	m_vUserBlockData.clear();
	m_vSearchRegData.clear();
	m_eUserBlockSoftType = ZXG_TDX_SOFT;

	m_aBreedName.RemoveAll();
	for (int iBreedNameIndex = 0; iBreedNameIndex < ZXG_BREED_Count + 1; iBreedNameIndex++)
	{
		switch(iBreedNameIndex)
		{
		case ZXG_BREED_SH:
			m_aBreedName.Add(_T("�Ϻ�֤ȯ"));
			break;
		case ZXG_BREED_SZ:
			m_aBreedName.Add(_T("����֤ȯ"));
			break;
		case ZXG_BREED_XG:
			m_aBreedName.Add(_T("���֤ȯ"));
		default:
			m_aBreedName.Add(_T("Ԥ��"));
			break;
		}
	}

};

CPubImportUserBlockFun::~CPubImportUserBlockFun()
{
	m_vUserBlockData.clear();
};


bool CPubImportUserBlockFun::AnalysisTdxUserBlockFile()
{
	m_vSearchRegData.clear();
	SearchSoftRegKey(ZXG_TDX_SOFT, REG_SYSTEM_32);
	SearchSoftRegKey(ZXG_TDX_SOFT, REG_SYSTEM_64);

	for (int i = 0; i < m_vSearchRegData.size(); ++i)
	{
		m_CurSoftPath = m_vSearchRegData.at(i).strAppPath;
		m_CurSoftName   = m_vSearchRegData.at(i).strDisplayName;
		m_eUserBlockSoftType = ZXG_TDX_SOFT;

		FindUserBlockFile(m_CurSoftPath, L"ZXG.blk");
	}
	return true;
}

bool CPubImportUserBlockFun::AnalysisDzhUserBlockFile()
{
	m_vSearchRegData.clear();
	SearchSoftRegKey(ZXG_DZH_SOFT, REG_SYSTEM_32);
	SearchSoftRegKey(ZXG_DZH_SOFT, REG_SYSTEM_64);

	for (int i = 0; i < m_vSearchRegData.size(); ++i)
	{
		m_CurSoftPath = m_vSearchRegData.at(i).strAppPath;
			m_CurSoftName   = m_vSearchRegData.at(i).strDisplayName;
		m_eUserBlockSoftType = ZXG_DZH_SOFT;

		FindUserBlockFile(m_CurSoftPath, L".blk");
	}
	return true;
}



bool CPubImportUserBlockFun::AnalysisThsUserBlockFile()
{
	m_vSearchRegData.clear();
	SearchSoftRegKey(ZXG_THS_SOFT, REG_SYSTEM_32);
	SearchSoftRegKey(ZXG_THS_SOFT, REG_SYSTEM_64);

	for (int i = 0; i < m_vSearchRegData.size(); ++i)
	{
		/*m_CurSoftName   = m_vSearchRegData.at(i).strDisplayName;
		m_CurSoftPath = m_vSearchRegData.at(i).strAppPath;
		m_eUserBlockSoftType = ZXG_THS_SOFT;

		FindUserBlockFile(m_CurSoftPath, L".blk");*/
	}
	return true;
}




CString CPubImportUserBlockFun::GetTdxUserBlockPath()
{
	m_CurSoftPath = _T("C:\\new_tdx\\");
	m_eUserBlockSoftType = ZXG_TDX_SOFT;
	m_CurSoftName   = _T("ͨ���Ž��ڿͻ���");
	return m_CurSoftPath;
}

CString CPubImportUserBlockFun::GetDzhUserBlockPath()
{
	m_CurSoftPath = _T("C:\\dzh365\\");
	m_eUserBlockSoftType = ZXG_DZH_SOFT;
	m_CurSoftName   = _T("���ǻ�365");
	return m_CurSoftPath;
}


CString CPubImportUserBlockFun::GetThsUserBlockPath()
{
	m_CurSoftPath = _T("C:\\ͬ��˳���\\ͬ��˳");
	m_eUserBlockSoftType = ZXG_THS_SOFT;
	m_CurSoftName   = _T("ͬ��˳");
	return m_CurSoftPath;
}



bool CPubImportUserBlockFun::AnalysisUserBlockFile(CString szSoftPath, CString szBlockPath)
{
	CFile cBlockFile;
	//char* pUserData;
	CString szUser;
	
	// ��ȡ�û���
	CString strTemp = szBlockPath;
	strTemp.Replace(szSoftPath, _T(""));
	szUser = strTemp.Mid(0, strTemp.FindOneOf(_T("\\")));
	
	// ��������д������̫����
	if (ZXG_TDX_SOFT == m_eUserBlockSoftType)
	{
		UserBlockAnalysisiMerch(szBlockPath, m_eUserBlockSoftType, szUser);
	}
	else if (ZXG_DZH_SOFT == m_eUserBlockSoftType)
	{
		DzhUserBlockAnalysisiMerch(szBlockPath, m_eUserBlockSoftType, szUser);
	}
	else if (ZXG_THS_SOFT == m_eUserBlockSoftType)
	{
		
	}

	
	return false;
}


void CPubImportUserBlockFun::FindUserBlockFile(CString szSoftPath, CString szFileEXT)
{
	CFileFind findFile ;
	CString strWildcard = szSoftPath + _T("*.*") ;

	BOOL fSuccess = findFile.FindFile(strWildcard) ;
	while (fSuccess)
	{
		fSuccess = findFile.FindNextFile() ;
		if (findFile.IsDots()) // ��ǰĿ¼���ϼ�Ŀ¼
		{
			//...
		}
		else if (findFile.IsDirectory())// ��Ŀ¼
		{
			CString czPath = findFile.GetFilePath();
			czPath += _T("\\");
			FindUserBlockFile(czPath, szFileEXT) ;
		}
		else
		{
			CString strFilePath = findFile.GetFilePath() ;
			
			if (-1 != strFilePath.Find(szFileEXT))
			{
				AnalysisUserBlockFile(m_CurSoftPath, strFilePath);	
			}
		}
	}

	findFile.Close() ;
	return; 
}



int HexToAsc(unsigned char *pDst, char *pSrc, int nSrcLen)
{
	for(int i=0; i<nSrcLen; i+=2)
	{
		// �����4λ
		if(*pSrc>='0' && *pSrc<='9')
		{
			*pDst = (*pSrc - '0') << 4;
		}
		else if(*pSrc>='A' && *pSrc<='F')
		{
			*pDst = (*pSrc - 'A' + 10) << 4;
		}
		else
		{
			*pDst = (*pSrc - 'a' + 10) << 4;
		}

		pSrc++;

		// �����4λ
		if(*pSrc>='0' && *pSrc<='9')
		{
			*pDst |= *pSrc - '0';
		}
		else if(*pSrc>='A' && *pSrc<='F')
		{
			*pDst |= *pSrc - 'A' + 10;
		}
		else
		{
			*pDst |= *pSrc - 'a' + 10;
		}

		pSrc++;
		pDst++;
	}
	// ����Ŀ�����ݳ���
	return nSrcLen / 2;
}


void CPubImportUserBlockFun::DzhUserBlockAnalysisiMerch(CString strUserBlockPath, E_UserBlock_SoftType eImPortType, CString strUser)
{
	////////���ǻ۵���ѡ�ļ���û����ѡ�ɹؼ��ֵĶ�����
	if (ZXG_DZH_SOFT == eImPortType)
	{
		if ( -1 == strUserBlockPath.Find(_T("��ѡ")))
		{
			return;
		}	
		if ( -1 != strUserBlockPath.Find(_T("backup")))
		{
			return;
		}	
		if ( -1 != strUserBlockPath.Find(_T("BACKUP")))
		{
			return;
		}	
	}

	// ������
	vector<CString> vUserBlockData;
	// ��Ʒ����
	vector<ST_MerchInfo> vMerInfoCount;
	CFile cReadFile;
	if (cReadFile.Open(strUserBlockPath, CFile::modeRead))
	{
		// a6 00 51 ff 01 53 48 30 30 30 30 30 31 00 00 00 00 ������Ҫ����12������������
		if (cReadFile.GetLength() < 12)
		{
			return;
		}

		CString strData;
		char *pBlkBuf = NULL;
		int iDataLen = cReadFile.GetLength();
		pBlkBuf = new char[cReadFile.GetLength()+1];
		memset(pBlkBuf, 0, cReadFile.GetLength()+1);
		cReadFile.Read(pBlkBuf, cReadFile.GetLength());
		cReadFile.Close();


		CStringA strBlkData = pBlkBuf;
		//int iLen = strlen(pBuf);
		char aStrSour[13] = { 0xa6,0x00,0x51,0xff,0x01 };
		char aCmpSign[5]  = {0};
		int iPosRead = 0;
		int iSignLen = 2;	// ������־�ĳ��ȣ�У�����ǽ�����־*2
		int iRightDataLen = 12;	// ��ȷ���ݵĳ���
		
		// �״β�Ҫ����ͷ
		iPosRead = 5;
		if (memcmp(aStrSour, pBlkBuf, iPosRead) != 0)
		{
			return;
		}
		//a6 00 51 ff 01 53 48 30 30 30 30 30 34 00 00 00 00 50 38 a4 58 53 48 30 30 30 30 30 35 00 00 00 00 50 38 a4
		memcpy(aCmpSign, pBlkBuf + iPosRead + iRightDataLen + iSignLen , iSignLen);
	
		for (iPosRead ; iPosRead < iDataLen;)
		{
			//��������
			//53 48 30 30 30 30 30 31 00 00 00 00 4b 8e 9d 58
			CString strData;
			strData = pBlkBuf + iPosRead;
		
			iPosRead = iPosRead + iRightDataLen + iSignLen + iSignLen;

			// �Ƚ�У����������λ�ǲ���a4��58
			memcpy(aStrSour, pBlkBuf + iPosRead - iSignLen , iSignLen);
			if (memcmp(aCmpSign, pBlkBuf + iPosRead - iSignLen, iSignLen) != 0)
			{
				break;
			}
			vUserBlockData.push_back(strData);
			
		}
		
	}

	for (int i = 0; i < vUserBlockData.size(); ++i)
	{
		CString strBreedName;
		CString strBlock = vUserBlockData.at(i);
	
		if (0 == strBlock.Find(L"SH"))
		{
			strBlock = strBlock.Mid(2);
			strBreedName = m_aBreedName.GetAt(ZXG_BREED_SH);
		}
		else if (0 == strBlock.Find(L"SZ"))
		{
			strBlock = strBlock.Mid(2);
			strBreedName = m_aBreedName.GetAt(ZXG_BREED_SZ);
		}
		else if (0 == strBlock.Find(L"HK"))
		{
			strBlock = strBlock.Mid(2);
			strBreedName = m_aBreedName.GetAt(ZXG_BREED_XG);
		}
		else
		{
			continue;
		}
		
		// �����ѡ��
		CMerch* pMerch   = NULL;
		if (FindTradeMerchByBreedNameAndCode(strBreedName, strBlock, pMerch))
		{
			if ( pMerch )
			{
				ST_MerchInfo  stMerchInfo;
				stMerchInfo.strMerchChName = pMerch->m_MerchInfo.m_StrMerchCnName;
				stMerchInfo.strMerchCode   = pMerch->m_MerchInfo.m_StrMerchCode;
				vMerInfoCount.push_back(stMerchInfo);
			}
		}
	}

	if (vMerInfoCount.size() > 0)
	{
		int iFindData = false;
		for (int i = 0; i < m_vUserBlockData.size(); ++i)
		{
			if (m_vUserBlockData.at(i).m_strSoftName == m_CurSoftName && m_vUserBlockData.at(i).m_strUser == strUser)
			{
				for (int j = 0; j < vMerInfoCount.size(); ++j)
				{
					m_vUserBlockData.at(i).m_vMerchInfo.push_back(vMerInfoCount.at(j));
				}
				iFindData = true;
			}
		}

		if (!iFindData)
		{
			CUserBlockData cUserBlockData;
			cUserBlockData.m_strSoftName = m_CurSoftName;
			cUserBlockData.m_strUser     = strUser;
			cUserBlockData.m_vMerchInfo.swap(vMerInfoCount);
			m_vUserBlockData.push_back(cUserBlockData);

		}
	}
}

//  ������ѡ���ļ�
void CPubImportUserBlockFun::UserBlockAnalysisiMerch(CString strUserBlockPath, E_UserBlock_SoftType eImPortType, CString strUser)
{
	// ��Ʒ����
	vector<ST_MerchInfo> vMerInfoCount;
	vMerInfoCount.clear();

	if (ZXG_TDX_SOFT == eImPortType)
	{
		// ��ȡ��Ʒ���������
		CString strBreedName;
		int iUserBlockIndex = 1;	

		vector<CString> vMerchCode;
		CStdioFile file;  
		if (file.Open(strUserBlockPath,CFile::modeRead |CFile::typeText))  
		{  
			file.SeekToBegin();//��λ���ļ���ͷ  
			CString strcode; 
			while (file.ReadString(strcode)) //��ȡ�ļ��е�һ��  
			{  
				vMerchCode.push_back(strcode); 
			}  
			file.Close();//�ر��ļ�  
		}  

		for (int i = 0; i < vMerchCode.size(); ++i)
		{
			CString strName = _T("");
			CString strCode = _T("");

			CString strMerchData = vMerchCode.at(i);

			if (0 == strCode.Find(L"31#") || 0 == strCode.Find(L"31#"))
			{
				// ȥ��31#����
				strCode = strCode.Mid(3);
				strBreedName = m_aBreedName.GetAt(ZXG_BREED_XG);
			}
			else
			{
				long long llMerchCode = atof(CStringA(strMerchData));;
				int iBreedId = llMerchCode/1000000;

				llMerchCode  = llMerchCode%1000000;
				strCode.Format(_T("%06lld"), llMerchCode);
				if (0 == iBreedId)	//ͨ���ŵ�0��������
				{
					strBreedName = m_aBreedName.GetAt(ZXG_BREED_SZ);
				}
				else if (1 == iBreedId) //ͨ���ŵ�1�����Ϻ�
				{
					strBreedName = m_aBreedName.GetAt(ZXG_BREED_SH);
				}
			}
			
			// �����ѡ��
			CMerch* pMerch   = NULL;
			if (FindTradeMerchByBreedNameAndCode(strBreedName, strCode, pMerch))
			{	
				if ( pMerch )
				{
					ST_MerchInfo  stMerchInfo;
					stMerchInfo.strMerchChName = pMerch->m_MerchInfo.m_StrMerchCnName;
					stMerchInfo.strMerchCode   = pMerch->m_MerchInfo.m_StrMerchCode;
					vMerInfoCount.push_back(stMerchInfo);
				}
			}
		}
	}
	else if (ZXG_THS_SOFT == eImPortType)
	{
		//.......
	}
	else if (ZXG_DZH_SOFT == eImPortType)
	{
		//.....
	}


	if (vMerInfoCount.size() > 0)
	{
		int iFindData = false;
		for (int i = 0; i < m_vUserBlockData.size(); ++i)
		{
			if (m_vUserBlockData.at(i).m_strSoftName == m_CurSoftName && m_vUserBlockData.at(i).m_strUser == strUser)
			{
				for (int j = 0; j < vMerInfoCount.size(); ++j)
				{
					m_vUserBlockData.at(i).m_vMerchInfo.push_back(vMerInfoCount.at(j));
				}

				// ����ά��Ч�ʣ�����һ���ظ����ݣ��ظ���ȥ��
				std::sort(m_vUserBlockData.at(i).m_vMerchInfo.begin(),m_vUserBlockData.at(i).m_vMerchInfo.end()); // ������  
				// ȥ���ظ���  
				std::vector<ST_MerchInfo>::iterator unque_it  = std::unique(m_vUserBlockData.at(i).m_vMerchInfo.begin(), m_vUserBlockData.at(i).m_vMerchInfo.end());  
				m_vUserBlockData.at(i).m_vMerchInfo.erase(unque_it, m_vUserBlockData.at(i).m_vMerchInfo.end());  
	
				iFindData = true;
			}
		}

		if (!iFindData)
		{
			CUserBlockData cUserBlockData;
			cUserBlockData.m_strSoftName = m_CurSoftName;
			cUserBlockData.m_strUser     = strUser;
			cUserBlockData.m_vMerchInfo.swap(vMerInfoCount);
			m_vUserBlockData.push_back(cUserBlockData);

		}
	}
}



BOOL CPubImportUserBlockFun::SearchSoftRegKey(E_UserBlock_SoftType eSoftType, E_REG_SYSTEM_TYPE eSystemType)
{

	TCHAR s_name[MAX_PATH] = {0};		// �����			
	TCHAR aRegData[MAX_PATH] = {0};					
	CString strRunExePathData;				// ִ���ļ�·��	
	CString strAppPathData;					// ����·��
	CString strDisplayNameData;				// �������


	DWORD cbName = MAX_PATH*sizeof(TCHAR);	
	DWORD cbName2 = sizeof(TCHAR);
	DWORD typeSZ = REG_SZ;			// SZ ע�������
	DWORD typeDWORD = REG_DWORD;	// DWORD ע�������
	HKEY hSubKey;
	DWORD dwIndex = 0;
	long lResult;

	HKEY hKey = HKEY_LOCAL_MACHINE;
	LPCTSTR szDesKeyItem = REG_KEY_ITEM;

	// ��ȡ��һ��ע�������������Ϣ
	CString strRunPath;
	CString strAppPath;
	CString strDisplayName;
	CString strRunExe;
	if (ZXG_TDX_SOFT == eSoftType)
	{
		strRunPath = REG_TDX_APP_EXE_PATH;
		strAppPath = REG_TDX_APP_PATH;
		strDisplayName = REG_DISPLAY_NAME;
		strRunExe   = REG_TDX_RUN_TOOL;
	}
	else if (ZXG_THS_SOFT == eSoftType)
	{
		strRunPath = REG_APP_EXE_PATH;
		strAppPath = REG_APP_PATH;
		strDisplayName = REG_DISPLAY_NAME;
		strRunExe   = REG_THS_RUN_TOOL;
	}
	else if (ZXG_DZH_SOFT == eSoftType)
	{
		strRunPath = REG_APP_EXE_PATH;
		strAppPath = REG_APP_PATH;
		strDisplayName = REG_DISPLAY_NAME;
		strRunExe   = REG_DZH_RUN_TOOL;
	}
	else
	{
		return FALSE;
	}
	
	if (REG_SYSTEM_64 == eSystemType)
	{
		lResult = RegOpenKeyEx(hKey, szDesKeyItem, 0, KEY_READ|KEY_WOW64_64KEY, &hSubKey);		// ��ע���
	}
	else
	{
		lResult = RegOpenKeyEx(hKey, szDesKeyItem, 0, KEY_READ, &hSubKey);		// ��ע���
	}
	if(ERROR_SUCCESS == lResult) 
	{
		//lResult = RegEnumKeyEx(hSubKey, dwIndex, s_name, &cbName, 0, NULL, NULL, NULL);		// ��ȡ�ֶ�
		while ((lResult = RegEnumKeyEx(hSubKey, dwIndex, s_name, &cbName, 0, NULL, NULL, NULL) == ERROR_SUCCESS) && lResult != ERROR_MORE_DATA)
		{
			dwIndex = dwIndex + 1;			// �ı�dwIndex��ѭ����ȡ�����ֶ�
			cbName = 256*sizeof(TCHAR);


			//OutputDebugString(s_name);
			HKEY hItem;		// ��һ��ע�����
			if(::RegOpenKeyEx(hSubKey, s_name, 0, KEY_READ, &hItem) == ERROR_SUCCESS)
			{		
#if 0
				DWORD i, retCode = ERROR_SUCCESS ; 
				TCHAR  achValue[MAX_VALUE_NAME]; 
				DWORD cchValue = MAX_VALUE_NAME; 
				for (i=0 ;retCode==ERROR_SUCCESS; i++) 
				{ 
					cchValue = MAX_VALUE_NAME; 
					achValue[0] = '\0'; 
					retCode = RegEnumValue(hItem, i, 
						achValue, 
						&cchValue, 
						NULL, 
						NULL,
						NULL,
						NULL);
					if (retCode == ERROR_SUCCESS ) 
					{ 
						strTemp.Format(TEXT("(%d) %s\n"), i+1, achValue);
						//OutputDebugString(strTemp);
					} 
				}
#endif

				memset(aRegData,0, MAX_PATH*sizeof(TCHAR));
				cbName = MAX_PATH*sizeof(TCHAR);
				if (::RegQueryValueEx(hItem, strRunPath, 0, &typeSZ, (LPBYTE)aRegData, &cbName) != ERROR_SUCCESS) 
				{
					continue;
				}
				strRunExePathData = aRegData;
				if (-1 == strRunExePathData.Find(strRunExe))
				{
					continue;
				}

				memset(aRegData,0, MAX_PATH*sizeof(TCHAR));
				cbName = MAX_PATH*sizeof(TCHAR);
				if (::RegQueryValueEx(hItem, strAppPath, 0, &typeSZ, (LPBYTE)aRegData, &cbName) != ERROR_SUCCESS) 
				{
					int i = GetLastError();
					continue;
				}
				strAppPathData = aRegData; 
				CString strPath = strAppPathData.Mid(strAppPathData.GetLength() -1);
				if (strPath.CompareNoCase(_T("\\")) != 0)
				{
					strAppPathData += "\\";
				}
				
				memset(aRegData,0, MAX_PATH*sizeof(TCHAR));
				cbName = MAX_PATH*sizeof(TCHAR);
				if (::RegQueryValueEx(hItem, strDisplayName, 0, &typeSZ, (LPBYTE)aRegData, &cbName) != ERROR_SUCCESS) 
				{
					continue;
				}
				strDisplayNameData = aRegData; 

				ST_SEARCH_REG_DATA stSearchData;
				stSearchData.strAppPath = strAppPathData;
				stSearchData.strRunPath = strRunExePathData;
				stSearchData.strDisplayName = strDisplayNameData;
				m_vSearchRegData.push_back(stSearchData);
			}
			//lResult = RegEnumKeyEx(hSubKey, dwIndex, s_name, &cbName, 0, NULL, NULL, NULL);
		}
	}
	RegCloseKey(hSubKey);	// �ر�һ��ע�����ȡ������Ϣ��
	RegCloseKey(hKey);

	if (m_vSearchRegData.size() > 0)
	{
		return TRUE;
	}
	return FALSE;
}
