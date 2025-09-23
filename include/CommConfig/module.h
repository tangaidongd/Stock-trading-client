#ifndef GLOBAL_TEMPLATE_HEAD_FILE
#define GLOBAL_TEMPLATE_HEAD_FILE

#pragma once

#include <windows.h>
#include <stdio.h>
/////////////////////////////////////////////////////////////////////////////////////////

//������Ϣ����
#define LEN_ERROR					256									//��Ϣ����
typedef void * (ModuleCreateFunc)();

//���������ģ��
template <typename IModeluInterface> class CTempldateHelper
{
	//��������
protected:
	LPCSTR							m_pszFuncName;						//��������
	LPCTSTR							m_pszModuleDll;						//�������
	HINSTANCE						m_hDllInstance;						//DLL ���
	IModeluInterface				* m_pIModeluInterface;				//ģ��ӿ�
	TCHAR							m_szErrorMsg[LEN_ERROR];			//������Ϣ

	//��������
public:
	//���캯��
	CTempldateHelper(LPCTSTR pszModuleDll, LPCSTR pszFuncName)
	{
		m_szErrorMsg[0]=0;
		m_hDllInstance=NULL;
		m_pIModeluInterface=NULL;
		m_pszFuncName=pszFuncName;
		m_pszModuleDll=pszModuleDll;
	}
	//��������
	virtual ~CTempldateHelper(void)
	{
		CloseInstance();
	}

	//���ܺ���
public:
	//��������
	bool CreateInstance()
	{
		CloseInstance();
		try
		{
			//���� DLL
			if(m_hDllInstance == NULL)
				m_hDllInstance=LoadLibrary(m_pszModuleDll);
			if (m_hDllInstance==NULL) 
			{
#ifdef UNICODE
				_snwprintf(m_szErrorMsg,sizeof(m_szErrorMsg),TEXT("%s ����ʧ��, �������:%ld!"),m_pszModuleDll,GetLastError());
#else
				_snprintf(m_szErrorMsg,sizeof(m_szErrorMsg),TEXT("%s ����ʧ��, �������:%ld!"),m_pszModuleDll,GetLastError());
#endif
				return false;
			}

			//Ѱ�ҵ�������
			ModuleCreateFunc * CreateFunc=(ModuleCreateFunc *)GetProcAddress(m_hDllInstance,m_pszFuncName);
			if (CreateFunc==NULL) 
			{
#ifdef UNICODE
				_snwprintf(m_szErrorMsg,sizeof(m_szErrorMsg),TEXT("%s ����������� %s ������, �������:%ld!"),m_pszModuleDll,m_pszFuncName,GetLastError());
#else
				_snprintf(m_szErrorMsg,sizeof(m_szErrorMsg),TEXT("%s ����������� %s ������, �������:%ld!"),m_pszModuleDll,m_pszFuncName,GetLastError());
#endif
				return false;
			}

			//�������
			m_pIModeluInterface=(IModeluInterface *)CreateFunc();
			if (m_pIModeluInterface==NULL) 
			{
#ifdef UNICODE
				_snwprintf(m_szErrorMsg,sizeof(m_szErrorMsg),TEXT("%s ����������� %s �������ʧ��!"),m_pszModuleDll,m_pszFuncName);
#else
				_snprintf(m_szErrorMsg,sizeof(m_szErrorMsg),TEXT("%s ����������� %s �������ʧ��!"),m_pszModuleDll,m_pszFuncName);
#endif
				return false;
			}
		}
		catch (...)	
		{ 
#ifdef UNICODE
			_snwprintf(m_szErrorMsg,sizeof(m_szErrorMsg),TEXT("%s ����������� %s �������쳣����!"),m_pszModuleDll,m_pszFuncName);
#else
			_snprintf(m_szErrorMsg,sizeof(m_szErrorMsg),TEXT("%s ����������� %s �������쳣����!"),m_pszModuleDll,m_pszFuncName);
#endif
			return false;
		}

		return true;
	}
	//�رպ���
	bool CloseInstance()
	{
		if (m_pIModeluInterface!=NULL)
		{
			m_pIModeluInterface->Release();
			m_pIModeluInterface=NULL;
		}
		if (m_hDllInstance!=NULL)
		{
			FreeLibrary(m_hDllInstance);
			m_hDllInstance=NULL;
		}
		return true;
	}
	//�Ƿ���Ч
	bool IsValid()
	{ 
		if (m_pIModeluInterface==NULL) return false;
		return true;
	}
	//��ȡ����
	LPCTSTR GetErrorMessage() { return m_szErrorMsg; }

	//��������
public:
	//ָ������
	IModeluInterface * operator->() {return GetInterface(); }
	//��ȡ�ӿ�
	IModeluInterface * GetInterface() { return m_pIModeluInterface; }
};

// �����в��������ڼ��DLL
typedef void * (ModuleCreateFuncCheck)(int iVer);

//���������ģ��
template <typename IModeluInterface> class CTempldateHelperCheck
{
	//��������
protected:
	LPCSTR							m_pszFuncName;						//��������
	LPCTSTR							m_pszModuleDll;						//�������
	HINSTANCE						m_hDllInstance;						//DLL ���
	IModeluInterface				* m_pIModeluInterface;				//ģ��ӿ�
	TCHAR							m_szErrorMsg[LEN_ERROR];			//������Ϣ
	int							m_iVer;						// �汾
	//��������
public:
	//���캯��
	CTempldateHelperCheck(LPCTSTR pszModuleDll, LPCSTR pszFuncName, int iVer)
	{
		m_szErrorMsg[0]=0;
		m_hDllInstance=NULL;
		m_pIModeluInterface=NULL;
		m_pszFuncName=pszFuncName;
		m_pszModuleDll=pszModuleDll;
		m_iVer = iVer;
	}
	//��������
	virtual ~CTempldateHelperCheck(void)
	{
		CloseInstance();
	}

	//���ܺ���
public:
	//��������
	bool CreateInstance()
	{
		CloseInstance();
		try
		{
			//���� DLL
			if(m_hDllInstance == NULL)
				m_hDllInstance=LoadLibrary(m_pszModuleDll);
			if (m_hDllInstance==NULL) 
			{
#ifdef UNICODE
				_snwprintf(m_szErrorMsg,sizeof(m_szErrorMsg),TEXT("%s ����ʧ��, �������:%ld!"),m_pszModuleDll,GetLastError());
#else
				_snprintf(m_szErrorMsg,sizeof(m_szErrorMsg),TEXT("%s ����ʧ��, �������:%ld!"),m_pszModuleDll,GetLastError());
#endif
				return false;
			}

			//Ѱ�ҵ�������
			ModuleCreateFuncCheck * CreateFunc=(ModuleCreateFuncCheck *)GetProcAddress(m_hDllInstance,m_pszFuncName);
			if (CreateFunc==NULL) 
			{
#ifdef UNICODE
				_snwprintf(m_szErrorMsg,sizeof(m_szErrorMsg),TEXT("%s ����������� %s ������, �������:%ld!"),m_pszModuleDll,m_pszFuncName,GetLastError());
#else
				_snprintf(m_szErrorMsg,sizeof(m_szErrorMsg),TEXT("%s ����������� %s ������, �������:%ld!"),m_pszModuleDll,m_pszFuncName,GetLastError());
#endif
				return false;
			}

			//�������
			m_pIModeluInterface=(IModeluInterface *)CreateFunc(m_iVer);
			if (m_pIModeluInterface==NULL) 
			{
#ifdef UNICODE
				_snwprintf(m_szErrorMsg,sizeof(m_szErrorMsg),TEXT("%s ����������� %s �������ʧ��!"),m_pszModuleDll,m_pszFuncName);
#else
				_snprintf(m_szErrorMsg,sizeof(m_szErrorMsg),TEXT("%s ����������� %s �������ʧ��!"),m_pszModuleDll,m_pszFuncName);
#endif
				return false;
			}
		}
		catch (...)	
		{ 
#ifdef UNICODE
			_snwprintf(m_szErrorMsg,sizeof(m_szErrorMsg),TEXT("%s ����������� %s �������쳣����!"),m_pszModuleDll,m_pszFuncName);
#else
			_snprintf(m_szErrorMsg,sizeof(m_szErrorMsg),TEXT("%s ����������� %s �������쳣����!"),m_pszModuleDll,m_pszFuncName);
#endif
			return false;
		}

		return true;
	}
	//�رպ���
	bool CloseInstance()
	{
		if (m_pIModeluInterface!=NULL)
		{
			m_pIModeluInterface->Release();
			m_pIModeluInterface=NULL;
		}
		if (m_hDllInstance!=NULL)
		{
			FreeLibrary(m_hDllInstance);
			m_hDllInstance=NULL;
		}
		return true;
	}
	//�Ƿ���Ч
	bool IsValid()
	{ 
		if (m_pIModeluInterface==NULL) return false;
		return true;
	}
	//��ȡ����
	LPCTSTR GetErrorMessage() { return m_szErrorMsg; }

	//��������
public:
	//ָ������
	IModeluInterface * operator->() {return GetInterface(); }
	//��ȡ�ӿ�
	IModeluInterface * GetInterface() { return m_pIModeluInterface; }
};
/////////////////////////////////////////////////////////////////////////////////////////

#endif