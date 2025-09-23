#pragma once
#include "zip.h" 
#include "unzip.h"
class AFX_EXT_CLASS CMyZip

{
public:
	CMyZip(void);
	~CMyZip(void);

private: 
	HZIP hz;						//Zip文件句柄 
	ZRESULT zr;						//操作返回值 
	ZIPENTRY ze;					//Zip文件入口 

	CString m_FolderPath;			//folder路径 
	CString  m_FolderName;			//folder将要被压缩的文件夹名 

private: 
	//实现遍历文件夹 
	void BrowseFile(CString &strFile); 

	//获取相对路径 
	void GetRelativePath(CString& pFullPath, CString& pSubString); 

	//创建路径 
	BOOL CreatedMultipleDirectory(wchar_t* direct); 
	
public:
	//压缩文件夹接口 
	BOOL Zip_PackFiles(CString& pFilePath, CString& mZipFileFullPath); 

	//解压缩文件夹接口 
	BOOL Zip_UnPackFiles(CString &mZipFileFullPath, CString& mUnPackPath, HWND hWnd = NULL); 

public: 
	//静态方法提供文件夹路径检查 
	static BOOL FolderExist(CString& strPath); 
};
