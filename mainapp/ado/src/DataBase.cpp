#include "StdAfx.h"
#include "DataBase.h"
#include "Recordset.h"

#include <time.h>
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
//////////////////////////////////////////////////////////////////////////

//���ݿ���������ε�������ʱ�Ĳ���
//lint --e{*}

//���캯��
CDataBase::CDataBase() : m_dwResumeConnectCount(10L),m_dwResumeConnectTime(10L)
{
	//״̬����
	m_dwConnectCount=0;
	m_dwConnectErrorTime=0L;

	//��������
	m_DBCommand.CreateInstance(__uuidof(Command));
	m_DBConnection.CreateInstance(__uuidof(Connection));

	//Ч������
	_ASSERT(m_DBCommand!=NULL);
	_ASSERT(m_DBConnection!=NULL);
	if (m_DBCommand==NULL) throw TEXT("���ݿ�������󴴽�ʧ��");	
	if (m_DBConnection==NULL) throw TEXT("���ݿ����Ӷ��󴴽�ʧ��");

	//���ñ���
	m_DBCommand->CommandType=adCmdStoredProc;

	return ;
}

//��������
CDataBase::~CDataBase()
{
	//�ر�����
	CloseConnection();

	//�ͷŶ���
	m_DBCommand.Release();
	
	m_DBConnection.Release();

	return;
}

//������
bool __cdecl CDataBase::OpenConnection()
{
	//�������ݿ�
	try
	{
		//�ر�����
		CloseConnection();

		//�������ݿ�
		EfficacyResult(m_DBConnection->Open(_bstr_t(m_strConnect), L"", L"", adConnectUnspecified));
		m_DBConnection->CursorLocation=adUseClient;
		m_DBCommand->ActiveConnection=m_DBConnection;

		//���ñ���
		m_dwConnectCount=0L;
		m_dwConnectErrorTime=0L;

		return true;
	}
	catch (CComError & ComError) { SetErrorInfo(ComError); }

	return false;
}



//�ر�����
bool __cdecl CDataBase::CloseConnection()
{
	try
	{
		if ((m_DBConnection!=NULL)&&(m_DBConnection->GetState()!=adStateClosed))
		{
			EfficacyResult(m_DBConnection->Close());
		}
		return true;
	}
	catch (CComError & ComError) { SetErrorInfo(ComError); }

	return false;
}

//��������
bool __cdecl CDataBase::TryConnectAgain(bool bFocusConnect, CComError * pComError)
{
	try
	{
		//�ж�����
		bool bReConnect=bFocusConnect;
		if (bReConnect==false)
		{
			DWORD dwNowTime=(DWORD)time(NULL);
			if ((m_dwConnectErrorTime+m_dwResumeConnectTime)>dwNowTime) bReConnect=true;
		}
		if ((bReConnect==false)&&(m_dwConnectCount>m_dwResumeConnectCount)) bReConnect=true;

		//���ñ���
		m_dwConnectCount++;
		m_dwConnectErrorTime=(DWORD)time(NULL);
		if (bReConnect==false)
		{
			if (pComError!=NULL) SetErrorInfo(*pComError);
			return false;
		}

		//��������
		OpenConnection();
		return true;
	}
	catch (CComError ComError)
	{
		//�������Ӵ���
		SetErrorInfo(ComError);
	}

	return false;
}

//������Ϣ
bool __cdecl CDataBase::SetConnectionInfo(LPCTSTR szIP, LPCTSTR szPipeName,
										  WORD wPort, LPCTSTR szData, LPCTSTR szName, LPCTSTR szPass)
{
	//Ч����� 
	_ASSERT(szIP!=NULL);
	_ASSERT(szData!=NULL);
	_ASSERT(szName!=NULL);
	_ASSERT(szPass!=NULL);

	//���������ַ���

	TCHAR szMsg[1024];
	//�ж����ݿ�ͨ��
	if(NULL == szPipeName || lstrlen(szPipeName)==0)
	{
		wsprintf(szMsg, TEXT("Provider=SQLOLEDB.1;Password=%s;Persist Security Info=True;\
											 User ID=%s;Initial Catalog=%s;Data Source=%s,%ld;"),
											 szPass,szName,szData,szIP,wPort);
	}
	else
	{
		wsprintf(szMsg, TEXT("Provider=SQLOLEDB.1;Password=%s;Persist Security Info=True;\
											 User ID=%s;Initial Catalog=%s;Data Source=%s\\%s,%ld;"),
											 szPass,szName,szData,szIP,szPipeName,wPort);
	}
	m_strConnect = szMsg;
	return true;
}

//�Ƿ����Ӵ���
bool __cdecl CDataBase::IsConnectError()
{
	try 
	{
		//״̬�ж�
		if (m_DBConnection==NULL) return true;
		if (m_DBConnection->GetState()==adStateClosed) return true;

		//�����ж�
		long lErrorCount=m_DBConnection->Errors->Count;
		if (lErrorCount>0L)
		{
	        ErrorPtr pError=NULL;
			for(long i=0;i<lErrorCount;i++)
			{
				pError=m_DBConnection->Errors->GetItem(i);
				// δָ���Ĵ���
				if (pError->Number==0x80004005) return true;
				// ��ʱ�ѹ���
				if (pError->Number==0x80040e31) return true;
			}
		}

		return false;
	}
	catch (CComError & ComError) { SetErrorInfo(ComError); }

	return false;
}


//��ȡ������ֵ
long __cdecl CDataBase::GetReturnValue()
{
	try 
	{
        _ParameterPtr Parameter;
		long lParameterCount=m_DBCommand->Parameters->Count;
		for (long i=0;i<lParameterCount;i++)
		{
			Parameter=m_DBCommand->Parameters->Item[i];
			if (Parameter->Direction==adParamReturnValue) return Parameter->Value.lVal;
		}
		_ASSERT(FALSE);
	}
	catch (CComError & ComError) { SetErrorInfo(ComError); }

	return 0;
}

//ɾ������
void __cdecl CDataBase::ClearAllParameters()
{
	try 
	{
		long lParameterCount=m_DBCommand->Parameters->Count;
		if (lParameterCount>0L)
		{
			for (long i=lParameterCount;i>0;i--)
			{
				//m_DBCommand->Parameters->Delete(i-1);
			}
		}
	}
	catch (CComError & ComError) { SetErrorInfo(ComError); }

	return;
}

//���ô洢����
void __cdecl CDataBase::SetSPName(LPCTSTR pszSpName)
{
	_ASSERT(pszSpName!=NULL);
	try 
	{ 
		m_DBCommand->CommandText=pszSpName; 
	}
	catch (CComError & ComError) { SetErrorInfo(ComError); }

	return;
}

//��ò���
void __cdecl CDataBase::GetParameterValue(LPCTSTR pszParamName, _variant_t & vtValue)
{
	//Ч�����
	_ASSERT(pszParamName!=NULL);

	//��ȡ����
	try 
	{
		vtValue.Clear();
		vtValue=m_DBCommand->Parameters->Item[pszParamName]->Value;
	}
	catch (CComError & ComError) { SetErrorInfo(ComError); }

	return;
}

//�������
void __cdecl CDataBase::AddParamter(LPCTSTR pszName, ADOCG::ParameterDirectionEnum Direction, ADOCG::DataTypeEnum Type, long lSize, const _variant_t & vtValue)
{
	_ASSERT(pszName!=NULL);
	try 
	{
        _ParameterPtr Parameter=m_DBCommand->CreateParameter(pszName,Type,Direction,lSize,vtValue);
		m_DBCommand->Parameters->Append(Parameter);
	}
	catch (CComError & ComError) { SetErrorInfo(ComError); }

	return;
}
//�������
void __cdecl CDataBase::AddDecimalParamter(LPCTSTR pszName, ADOCG::ParameterDirectionEnum Direction, DOUBLE& dbValue, 
										   unsigned char Precision , unsigned char NumericScale)
{
	DECIMAL dec;
	VarDecFromR8(dbValue, &dec);
	AddDecimalParamter(pszName, Direction, dec, Precision, NumericScale);
}
//�������
void __cdecl CDataBase::AddDecimalParamter(LPCTSTR pszName, ADOCG::ParameterDirectionEnum Direction, DECIMAL& decValue, 
										   unsigned char Precision , unsigned char NumericScale)
{
	_ASSERT(pszName!=NULL);
	try 
	{
		_ParameterPtr Parameter=m_DBCommand->CreateParameter(pszName,adDecimal,Direction,sizeof(DECIMAL),_variant_t(decValue));
		Parameter->NumericScale = NumericScale;
		Parameter->Precision    = Precision;
		m_DBCommand->Parameters->Append(Parameter);
	}
	catch (CComError & ComError) { SetErrorInfo(ComError); }
}
//ִ�����
bool __cdecl CDataBase::Execute(LPCTSTR pszCommand, IN OUT CRecordSet *pRecordSet)
{
	_ASSERT(pszCommand!=NULL);
	try
	{
		m_DBConnection->CursorLocation=adUseClient;
		if (NULL != pRecordSet)
		{
			pRecordSet->m_DBRecordset->Open(pszCommand, 
											_variant_t((IDispatch*)m_DBConnection, true),
											adOpenUnspecified, adLockUnspecified, adOptionUnspecified);
		} 
		else
		{
			m_DBConnection->Execute(pszCommand,NULL,adExecuteNoRecords);
		}
		return true;
	}
	catch (CComError & ComError) 
	{ 
		if (IsConnectError()==true)	TryConnectAgain(false,&ComError);
		else SetErrorInfo(ComError);
	}

	return false;
}

//ִ������
bool __cdecl CDataBase::ExecuteCommand(IN OUT CRecordSet *pRecordSet)
{
	try 
	{

		//ִ������
		if (NULL != pRecordSet)
		{
			pRecordSet->m_DBRecordset->PutRefSource(m_DBCommand);
			pRecordSet->m_DBRecordset->CursorLocation=adUseClient;
			EfficacyResult(pRecordSet->m_DBRecordset->Open((IDispatch *)m_DBCommand,vtMissing,adOpenForwardOnly,adLockReadOnly,adOptionUnspecified));
		}
		else 
		{
			m_DBConnection->CursorLocation=adUseClient;
			EfficacyResult(m_DBCommand->Execute(NULL,NULL,adExecuteNoRecords));
		}
		return true;
	}
	catch (CComError & ComError) 
	{ 
		if (IsConnectError()==true)	TryConnectAgain(false,&ComError);
		else SetErrorInfo(ComError);
	}
	return false;
}

//��ȡ����
LPCTSTR CDataBase::GetComErrorDescribe()
{
	return m_strErrorDescribe;
}

_ConnectionPtr CDataBase::GetConnection()
{
	return m_DBConnection;
}

//���ô���
void CDataBase::SetErrorInfo(CComError & ComError)
{
	_bstr_t bstrDescribe(ComError.Description());
	TCHAR szMsg[1024];
	wsprintf(szMsg, TEXT("ADO ����0x%8x��%s"),ComError.Error(),(LPCTSTR)bstrDescribe);
	m_strErrorDescribe = szMsg;
}

// ��ʼ��COM����
bool CDataBase::InitComEnv()
{
	if(FAILED(CoInitialize(NULL)))
	{
		return false;
	}
	return true;
}

void CDataBase::UnitComEnv()
{
	CoUninitialize();
}

//д�������
bool CDataBase::WriteBinary(IN LPCTSTR lpSelect, IN LPCTSTR lpFieldName, IN const char *pBinaryData, IN int iDataLen)
{
	bool bSuc = true;
	_RecordsetPtr m_pRecordset;
	try
	{					
		m_pRecordset.CreateInstance("ADODB.Recordset");
		m_pRecordset->Open(lpSelect, _variant_t((IDispatch*)GetConnection(), true), adOpenStatic, adLockOptimistic, adCmdText);

		int nSize = iDataLen;
		const char *pData = pBinaryData;

		VARIANT			varBLOB;
		SAFEARRAY		*psa;
		SAFEARRAYBOUND	rgsabound[1];
		rgsabound[0].lLbound = 0;
		rgsabound[0].cElements = nSize;
		psa = SafeArrayCreate(VT_UI1, 1, rgsabound);
		for (long i = 0; i < (long)nSize; i++)
		{
			SafeArrayPutElement(psa, &i, (void *)pData++);
		}
		varBLOB.vt = VT_ARRAY | VT_UI1;
		varBLOB.parray = psa;
		m_pRecordset->GetFields()->GetItem(lpFieldName)->AppendChunk(varBLOB);

		m_pRecordset->Update();

		SafeArrayDestroy(psa);
		psa = NULL;
	}
	catch(_com_error e)
	{
		bSuc = false;
	}
	if (NULL != m_pRecordset.GetInterfacePtr())
	{
		m_pRecordset->Close();
	}
	m_pRecordset.Release();
	return bSuc;
}