#pragma once
#include "zip.h" 
#include "unzip.h"
class AFX_EXT_CLASS CMyZip

{
public:
	CMyZip(void);
	~CMyZip(void);

private: 
	HZIP hz;						//Zip�ļ���� 
	ZRESULT zr;						//��������ֵ 
	ZIPENTRY ze;					//Zip�ļ���� 

	CString m_FolderPath;			//folder·�� 
	CString  m_FolderName;			//folder��Ҫ��ѹ�����ļ����� 

private: 
	//ʵ�ֱ����ļ��� 
	void BrowseFile(CString &strFile); 

	//��ȡ���·�� 
	void GetRelativePath(CString& pFullPath, CString& pSubString); 

	//����·�� 
	BOOL CreatedMultipleDirectory(wchar_t* direct); 
	
public:
	//ѹ���ļ��нӿ� 
	BOOL Zip_PackFiles(CString& pFilePath, CString& mZipFileFullPath); 

	//��ѹ���ļ��нӿ� 
	BOOL Zip_UnPackFiles(CString &mZipFileFullPath, CString& mUnPackPath, HWND hWnd = NULL); 

public: 
	//��̬�����ṩ�ļ���·����� 
	static BOOL FolderExist(CString& strPath); 
};
