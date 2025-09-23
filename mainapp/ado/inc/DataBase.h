#ifndef _DATA_BASE_H_
#define _DATA_BASE_H_

#pragma once

#include "Recordset.h"
#include <string>

using namespace std;

//////////////////////////////////////////////////////////////////////////

//���ݿ����
class CDataBase
{
	friend CRecordSet;
	//��Ϣ����
protected:
	CString							m_strConnect;						//�����ַ���
	CString							m_strErrorDescribe;					//������Ϣ
	
	//״̬����
protected:
	DWORD							m_dwConnectCount;					//���Դ���
	DWORD							m_dwConnectErrorTime;				//����ʱ��
	const DWORD						m_dwResumeConnectCount;				//�ָ�����
	const DWORD						m_dwResumeConnectTime;				//�ָ�ʱ��
	
	//�ں˱���
protected:
	_CommandPtr						m_DBCommand;						//�������
	_ConnectionPtr					m_DBConnection;						//���ݿ����
	
	//��������
public:
	//���캯��
	CDataBase();
	//��������
	virtual ~CDataBase();
	
	//����ӿ�
public:
	//������
	virtual bool __cdecl OpenConnection();
	
	//�ر�����
	virtual bool __cdecl CloseConnection();
	//��������
	virtual bool __cdecl TryConnectAgain(bool bFocusConnect, CComError * pComError);
	//������Ϣ
	virtual bool __cdecl SetConnectionInfo(LPCTSTR szIP,LPCTSTR szPipeName, WORD wPort, LPCTSTR szData, LPCTSTR szName, LPCTSTR szPass);
	
	//״̬�ӿ�
public:
	//�Ƿ����Ӵ���
	virtual bool __cdecl IsConnectError();
	
	//�������ӿ�
public:
	//���ô洢����
	virtual void __cdecl SetSPName(LPCTSTR pszSpName);
	//�������
	virtual void __cdecl AddParamter(LPCTSTR pszName, ADOCG::ParameterDirectionEnum Direction, ADOCG::DataTypeEnum Type, long lSize, const _variant_t & vtValue);
	//�������
	virtual void __cdecl AddDecimalParamter(LPCTSTR pszName, ADOCG::ParameterDirectionEnum Direction, DOUBLE& dbValue, 
		unsigned char Precision = 24, unsigned char NumericScale = 4);
	//�������
	virtual void __cdecl AddDecimalParamter(LPCTSTR pszName, ADOCG::ParameterDirectionEnum Direction, DECIMAL& decValue, 
		unsigned char Precision = 24, unsigned char NumericScale = 4);
	//ɾ������
	virtual void __cdecl ClearAllParameters();
	//��ò���
	virtual void __cdecl GetParameterValue(LPCTSTR pszParamName, _variant_t & vtValue);
	//��ȡ������ֵ
	virtual long __cdecl GetReturnValue();
	
	//ִ�нӿ�
public:
	//ִ�����
	virtual bool __cdecl Execute(LPCTSTR pszCommand, IN OUT CRecordSet *pRecordSet);
	//ִ������
	virtual bool __cdecl ExecuteCommand(IN OUT CRecordSet *pRecordSet);
	//д�������
	virtual bool __cdecl WriteBinary(IN LPCTSTR lpSelect, IN LPCTSTR lpFieldName, IN const char *pBinaryData, IN int iDataLen);
	// ��ʼ��COM����
	static bool InitComEnv();
	
	static void UnitComEnv();
	//��ȡ����
	LPCTSTR GetComErrorDescribe();
	//�ڲ�����
	
	_ConnectionPtr GetConnection();
private:
	
	//���ô���
	void SetErrorInfo(CComError & ComError);
};
#endif