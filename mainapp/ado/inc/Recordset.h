#ifndef _RECORDSET_H_
#define _RECORDSET_H_

//lint --e(322)
#pragma warning(disable:4146) 
#import "C:\Program Files\Common Files\System\ado\msado15.dll" rename_namespace("ADOCG") rename("EOF","EndOfFile")
#pragma warning(default:4146) 

#include <string>
//#include <atlcomtime.h>
#include <windows.h>

using namespace ADOCG;

typedef _com_error					CComError;							//COM 错误

using std::string;

#define EfficacyResult(hResult) { if (FAILED(hResult)) _com_issue_error(hResult); }

class CDataBase;
class CRecordSet
{
	friend CDataBase;
	
	CString							m_strConnect;						//连接字符串
	CString							m_strErrorDescribe;					//错误信息
	
protected:
	_RecordsetPtr					m_DBRecordset;						//记录集对象
	
private:
	//设置错误
	void SetErrorInfo(CComError & ComError);
	
public:
	CRecordSet();
	
	~CRecordSet();
	
	//是否打开
	bool __cdecl IsRecordsetOpened();
	
	//关闭记录
	virtual bool __cdecl CloseRecordset();
	
	//获取错误
	LPCTSTR GetComErrorDescribe();
	
	//往下移动
	void __cdecl MoveToNext();
	//移到开头
	void __cdecl MoveToFirst();
	//是否结束
	bool __cdecl IsEndRecordset();
	//获取数目
	long __cdecl GetRecordCount();
	//获取大小
	long __cdecl GetActualSize(LPCTSTR pszParamName);
	//下一记录集
	bool __cdecl NextRecordset();
	
	//字段接口
	
	//获取参数
	bool __cdecl GetFieldValue(LPCTSTR lpFieldName, BYTE & bValue);
	//获取参数
	bool __cdecl GetFieldValue(LPCTSTR lpFieldName, WORD & wValue);
	//获取参数
	bool __cdecl GetFieldValue(LPCTSTR lpFieldName, INT & nValue);
	//获取参数
	bool __cdecl GetFieldValue(LPCTSTR lpFieldName, LONG & lValue);
	//获取参数
	bool __cdecl GetFieldValue(LPCTSTR lpFieldName, DWORD & ulValue);
	//获取参数
	bool __cdecl GetFieldValue(LPCTSTR lpFieldName, UINT & ulValue);
	//获取参数
	bool __cdecl GetFieldValue(LPCTSTR lpFieldName, DOUBLE & dbValue);
	//获取参数
	bool __cdecl GetFieldValue(LPCTSTR lpFieldName, __int64 & llValue);
	//获取参数
	bool __cdecl GetFieldValue(LPCTSTR lpFieldName, ULONGLONG & ullValue);
	//获取参数
	UINT __cdecl GetFieldValue(LPCTSTR lpFieldName, TCHAR szBuffer[], UINT uSize);
	//获取参数
	bool __cdecl GetFieldValue(LPCTSTR lpFieldName, COleDateTime & Time);
	//获取参数
	bool __cdecl GetFieldValue(LPCTSTR lpFieldName, bool & bValue);
	//获取参数
	bool __cdecl GetFieldValue(LPCTSTR lpFieldName, DECIMAL & decValue);
};


#endif