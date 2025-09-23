#include "StdAfx.h"
//#include "Recordset.h"
//#include "..\ado\inc\DataBase.h"
#include "..\..\ado\inc\Recordset.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif	

//数据库操作类屏蔽掉错误，暂时改不掉
//lint --e{*}

CRecordSet::CRecordSet()
{
	m_DBRecordset.CreateInstance(__uuidof(Recordset));
	_ASSERT(m_DBRecordset!=NULL);
	if (m_DBRecordset==NULL) throw TEXT("数据库记录集对象创建失败");
}

CRecordSet::~CRecordSet()
{
	m_DBRecordset.Release();
}


//是否打开
bool __cdecl CRecordSet::IsRecordsetOpened()
{

	if (m_DBRecordset==NULL) return false;
	if (m_DBRecordset->GetState()==adStateClosed) return false;
	return true;
}

//关闭记录
bool __cdecl CRecordSet::CloseRecordset()
{
	try 
	{
		if (IsRecordsetOpened()) EfficacyResult(m_DBRecordset->Close());
		return true;
	}
	catch (CComError & ComError) { SetErrorInfo(ComError); }

	return false;
}

//往下移动
void __cdecl CRecordSet::MoveToNext()
{
	try 
	{ 
		m_DBRecordset->MoveNext(); 
	}
	catch (CComError & ComError) { SetErrorInfo(ComError); }

	return;
}

//移到开头
void __cdecl CRecordSet::MoveToFirst()
{
	try 
	{ 
		m_DBRecordset->MoveFirst(); 
	}
	catch (CComError & ComError) { SetErrorInfo(ComError); }

	return;
}

//是否结束
bool __cdecl CRecordSet::IsEndRecordset()
{
	try 
	{
		return (m_DBRecordset->EndOfFile==VARIANT_TRUE); 
	}
	catch (CComError & ComError) { SetErrorInfo(ComError); }

	return true;
}

//获取数目
long __cdecl CRecordSet::GetRecordCount()
{
	try
	{
		if (m_DBRecordset==NULL) return 0;
		return m_DBRecordset->GetRecordCount();
	}
	catch (CComError & ComError) { SetErrorInfo(ComError); }

	return 0;
}

//获取大小
long __cdecl CRecordSet::GetActualSize(LPCTSTR pszParamName)
{
	_ASSERT(pszParamName!=NULL);
	try 
	{
		return m_DBRecordset->Fields->Item[pszParamName]->ActualSize;
	}
	catch (CComError & ComError) { SetErrorInfo(ComError); }

	return -1;
}

//下一记录集
bool __cdecl CRecordSet::NextRecordset()
{
	_ASSERT(m_DBRecordset != NULL);
	try 
	{
		m_DBRecordset = m_DBRecordset->NextRecordset(NULL);

		return IsRecordsetOpened();
	}
	catch (CComError & ComError) { SetErrorInfo(ComError); }

	return false;
}
//获取参数
bool __cdecl CRecordSet::GetFieldValue(LPCTSTR lpFieldName, BYTE & bValue)
{
	try
	{
		bValue=0;
		_variant_t vtFld=m_DBRecordset->Fields->GetItem(lpFieldName)->Value;
		switch(vtFld.vt)
		{
		case VT_BOOL:
			{
				bValue=(vtFld.boolVal!=0)?1:0;
				break;
			}
		case VT_I2:
		case VT_UI1:
			{
				bValue=vtFld.iVal;
				break;
			}
		case VT_NULL:
		case VT_EMPTY:
			{
				bValue=0;
				break;
			}
		default: bValue=(BYTE)vtFld.iVal;
		}	
		return true;
	}
	catch (CComError & ComError) { SetErrorInfo(ComError); }

	return false;
}

//获取参数
bool __cdecl CRecordSet::GetFieldValue(LPCTSTR lpFieldName, UINT & ulValue)
{
	try
	{
		ulValue=0L;
		_variant_t vtFld=m_DBRecordset->Fields->GetItem(lpFieldName)->Value;
		if ((vtFld.vt!=VT_NULL)&&(vtFld.vt!=VT_EMPTY)) ulValue=vtFld.lVal;
		return true;
	}
	catch (CComError & ComError) { SetErrorInfo(ComError); }

	return false;
}

//获取参数
bool __cdecl CRecordSet::GetFieldValue(LPCTSTR lpFieldName, DOUBLE & dbValue)
{	
	try
	{
		dbValue=0.0L;
		_variant_t vtFld=m_DBRecordset->Fields->GetItem(lpFieldName)->Value;
		switch(vtFld.vt)
		{
		case VT_R4:
			{
				dbValue=vtFld.fltVal;
				break;
			}
		case VT_R8:
			{
				dbValue=vtFld.dblVal;
				break;
			}
		case VT_DECIMAL:
			{
				//dbValue=vtFld.decVal.Lo64;
				//dbValue*=(vtFld.decVal.sign==128)?-1:1;
				//dbValue/=pow((float)10,vtFld.decVal.scale); 
				VarR8FromDec(&vtFld.decVal, &dbValue);
				break;
			}
		case VT_UI1:
			{
				dbValue=vtFld.iVal;
				break;
			}
		case VT_I2:
		case VT_I4:
			{
				dbValue=vtFld.lVal;
				break;
			}
		case VT_NULL:
		case VT_EMPTY:
			{
				dbValue=0.0L;
				break;
			}
		default: dbValue=vtFld.dblVal;
		}
		return true;
	}
	catch (CComError & ComError) { SetErrorInfo(ComError); }

	return false;
}

//获取参数
bool __cdecl CRecordSet::GetFieldValue(LPCTSTR lpFieldName, LONG & lValue)
{
	try
	{
		lValue=0L;
		_variant_t vtFld=m_DBRecordset->Fields->GetItem(lpFieldName)->Value;
		if ((vtFld.vt!=VT_NULL)&&(vtFld.vt!=VT_EMPTY)) lValue=vtFld.lVal;
		return true;
	}
	catch (CComError & ComError) { SetErrorInfo(ComError); }

	return false;
}

//获取参数
bool __cdecl CRecordSet::GetFieldValue(LPCTSTR lpFieldName, DWORD & ulValue)
{
	try
	{
		ulValue=0L;
		_variant_t vtFld=m_DBRecordset->Fields->GetItem(lpFieldName)->Value;
		if ((vtFld.vt!=VT_NULL)&&(vtFld.vt!=VT_EMPTY)) ulValue=vtFld.ulVal;
		return true;
	}
	catch (CComError & ComError) { SetErrorInfo(ComError); }

	return false;
}

//获取参数
bool __cdecl CRecordSet::GetFieldValue(LPCTSTR lpFieldName, INT & nValue)
{
	try
	{
		nValue=0;
		_variant_t vtFld = m_DBRecordset->Fields->GetItem(lpFieldName)->Value;
		switch(vtFld.vt)
		{
		case VT_INT:
			{
				nValue = vtFld.intVal;
				break;
			}			
		case VT_BOOL:
			{
				nValue = vtFld.boolVal;
				break;
			}
		case VT_I2:
		case VT_UI1:
			{
				nValue = vtFld.iVal;
				break;
			}
		case VT_NULL:
		case VT_EMPTY:
			{
				nValue = 0;
				break;
			}
		default: nValue = vtFld.iVal;
		}	
		return true;
	}
	catch (CComError & ComError) { SetErrorInfo(ComError); }

	return false;
}

//获取参数
bool __cdecl CRecordSet::GetFieldValue(LPCTSTR lpFieldName, __int64 & llValue)
{
	try
	{
		llValue=0L;
		_variant_t vtFld=m_DBRecordset->Fields->GetItem(lpFieldName)->Value;
		//if ((vtFld.vt!=VT_NULL)&&(vtFld.vt!=VT_EMPTY)) llValue=vtFld.llVal;
		return true;
	}
	catch (CComError & ComError) { SetErrorInfo(ComError); }

	return false;
}
//获取参数
bool __cdecl CRecordSet::GetFieldValue(LPCTSTR lpFieldName, ULONGLONG & ullValue)
{
	try
	{
		ullValue=0L;
		_variant_t vtFld=m_DBRecordset->Fields->GetItem(lpFieldName)->Value;
		//if ((vtFld.vt!=VT_NULL)&&(vtFld.vt!=VT_EMPTY)) ullValue=vtFld.llVal;
		return true;
	}
	catch (CComError & ComError) { SetErrorInfo(ComError); }

	return false;
}
//获取参数
bool __cdecl CRecordSet::GetFieldValue(LPCTSTR lpFieldName, DECIMAL & decValue)
{	
	try
	{
		_variant_t vtFld=m_DBRecordset->Fields->GetItem(lpFieldName)->Value;
		switch(vtFld.vt)
		{
		case VT_R4:
			{
				VarDecFromR4(vtFld.fltVal,&decValue);
				break;
			}
		case VT_R8:
			{
				VarDecFromR8(vtFld.dblVal,&decValue);
				break;
			}
		case VT_DECIMAL:
			{
				memcpy(&decValue, &vtFld.decVal, sizeof(DECIMAL));
				break;
			}
		case VT_UI1:
			{
				VarDecFromUI1(vtFld.bVal,&decValue);
				break;
			}
		case VT_I2:
		case VT_I4:
			{
				VarDecFromUI4(vtFld.lVal,&decValue);
				break;
			}
		case VT_NULL:
		case VT_EMPTY:
			{
				memset(&decValue, 0, sizeof(DECIMAL));
				break;
			}
		default: VarDecFromR8(vtFld.dblVal,&decValue);
		}
		return true;
	}
	catch (CComError & ComError) { SetErrorInfo(ComError); }

	return false;
}

inline void KillBlankChar(TCHAR szBuffer[], UINT uSize)
{
	for(int i = uSize-1; i>=0; i --)
	{
		if(szBuffer[i] == 32)
		{
			szBuffer[i] = 0;
		}
		else
			break;
	}
}

//获取参数
UINT __cdecl CRecordSet::GetFieldValue(LPCTSTR lpFieldName, TCHAR szBuffer[], UINT uSize)
{
	try
	{
		_variant_t vtFld=m_DBRecordset->Fields->GetItem(lpFieldName)->Value;
		int DataSize = m_DBRecordset->Fields->GetItem(lpFieldName)->ActualSize;
		if (vtFld.vt==VT_BSTR)
		{
			lstrcpy(szBuffer, (TCHAR*)_bstr_t(vtFld));
			KillBlankChar(szBuffer, DataSize > uSize ? uSize : DataSize);
			return true;
		}
		else if(vtFld.vt == (VT_ARRAY|VT_UI1))
		{
			if(DataSize)
			{
				char *buff;
				SafeArrayAccessData(vtFld.parray,(void**)&buff);
				if(DataSize > uSize)
				{
					memcpy(szBuffer, buff, uSize);
					szBuffer[uSize] = '\0';
				}
				else
				{
					memcpy(szBuffer, buff, DataSize);
					szBuffer[DataSize] = '\0';
				}				
				SafeArrayUnaccessData(vtFld.parray);
			}

			return DataSize > uSize ? uSize : DataSize;
		}
		else
		{
			szBuffer[DataSize] = '\0';
		}
		return (UINT)-1;
	}
	catch (CComError & ComError) { SetErrorInfo(ComError); }

	return (UINT)-1;
}

//获取参数
bool __cdecl CRecordSet::GetFieldValue(LPCTSTR lpFieldName, WORD & wValue)
{
	try
	{
		wValue=0L;
		_variant_t vtFld=m_DBRecordset->Fields->GetItem(lpFieldName)->Value;
		if ((vtFld.vt!=VT_NULL)&&(vtFld.vt!=VT_EMPTY)) wValue=(WORD)vtFld.ulVal;
		return true;
	}
	catch (CComError & ComError) { SetErrorInfo(ComError); }

	return false;
}

//获取参数
bool __cdecl CRecordSet::GetFieldValue(LPCTSTR lpFieldName, COleDateTime & Time)
{
	try
	{
		_variant_t vtFld = m_DBRecordset->Fields->GetItem(lpFieldName)->Value;
		switch(vtFld.vt) 
		{
		case VT_DATE:
			{
				COleDateTime TempTime(vtFld);
				Time=TempTime;
				break;
			}
		case VT_EMPTY:
		case VT_NULL:
			{
				Time.SetStatus(COleDateTime::null);
				break;
			}
		default: return false;
		}
		return true;
	}
	catch (CComError & ComError) { SetErrorInfo(ComError); }

	return false;
}

//获取参数
bool __cdecl CRecordSet::GetFieldValue(LPCTSTR lpFieldName, bool & bValue)
{
	try
	{
		_variant_t vtFld=m_DBRecordset->Fields->GetItem(lpFieldName)->Value;
		switch(vtFld.vt) 
		{
		case VT_BOOL:
			{
				bValue=(vtFld.boolVal==0)?false:true;
				break;
			}
		case VT_EMPTY:
		case VT_NULL:
			{
				bValue = false;
				break;
			}
		default:return false;
		}
		return true;
	}
	catch (CComError & ComError) { SetErrorInfo(ComError); }

	return false;
}

//获取错误
LPCTSTR CRecordSet::GetComErrorDescribe()
{
	return m_strErrorDescribe;
}

//设置错误
void CRecordSet::SetErrorInfo(CComError & ComError)
{
	_bstr_t bstrDescribe(ComError.Description());
	TCHAR szMsg[1024];
	wsprintf(szMsg, TEXT("ADO 错误：0x%8x，%s"),ComError.Error(),(LPCTSTR)bstrDescribe);
	m_strErrorDescribe = szMsg;
}