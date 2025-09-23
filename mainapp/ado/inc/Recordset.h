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

typedef _com_error					CComError;							//COM ����

using std::string;

#define EfficacyResult(hResult) { if (FAILED(hResult)) _com_issue_error(hResult); }

class CDataBase;
class CRecordSet
{
	friend CDataBase;
	
	CString							m_strConnect;						//�����ַ���
	CString							m_strErrorDescribe;					//������Ϣ
	
protected:
	_RecordsetPtr					m_DBRecordset;						//��¼������
	
private:
	//���ô���
	void SetErrorInfo(CComError & ComError);
	
public:
	CRecordSet();
	
	~CRecordSet();
	
	//�Ƿ��
	bool __cdecl IsRecordsetOpened();
	
	//�رռ�¼
	virtual bool __cdecl CloseRecordset();
	
	//��ȡ����
	LPCTSTR GetComErrorDescribe();
	
	//�����ƶ�
	void __cdecl MoveToNext();
	//�Ƶ���ͷ
	void __cdecl MoveToFirst();
	//�Ƿ����
	bool __cdecl IsEndRecordset();
	//��ȡ��Ŀ
	long __cdecl GetRecordCount();
	//��ȡ��С
	long __cdecl GetActualSize(LPCTSTR pszParamName);
	//��һ��¼��
	bool __cdecl NextRecordset();
	
	//�ֶνӿ�
	
	//��ȡ����
	bool __cdecl GetFieldValue(LPCTSTR lpFieldName, BYTE & bValue);
	//��ȡ����
	bool __cdecl GetFieldValue(LPCTSTR lpFieldName, WORD & wValue);
	//��ȡ����
	bool __cdecl GetFieldValue(LPCTSTR lpFieldName, INT & nValue);
	//��ȡ����
	bool __cdecl GetFieldValue(LPCTSTR lpFieldName, LONG & lValue);
	//��ȡ����
	bool __cdecl GetFieldValue(LPCTSTR lpFieldName, DWORD & ulValue);
	//��ȡ����
	bool __cdecl GetFieldValue(LPCTSTR lpFieldName, UINT & ulValue);
	//��ȡ����
	bool __cdecl GetFieldValue(LPCTSTR lpFieldName, DOUBLE & dbValue);
	//��ȡ����
	bool __cdecl GetFieldValue(LPCTSTR lpFieldName, __int64 & llValue);
	//��ȡ����
	bool __cdecl GetFieldValue(LPCTSTR lpFieldName, ULONGLONG & ullValue);
	//��ȡ����
	UINT __cdecl GetFieldValue(LPCTSTR lpFieldName, TCHAR szBuffer[], UINT uSize);
	//��ȡ����
	bool __cdecl GetFieldValue(LPCTSTR lpFieldName, COleDateTime & Time);
	//��ȡ����
	bool __cdecl GetFieldValue(LPCTSTR lpFieldName, bool & bValue);
	//��ȡ����
	bool __cdecl GetFieldValue(LPCTSTR lpFieldName, DECIMAL & decValue);
};


#endif