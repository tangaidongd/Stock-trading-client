#ifndef GLOBAL_TEMPLATE_HEAD_FILE
#define GLOBAL_TEMPLATE_HEAD_FILE

#pragma once

#include <windows.h>
#include <stdio.h>
/////////////////////////////////////////////////////////////////////////////////////////

//错误信息长度
#define LEN_ERROR					256									//信息长度
typedef void * (ModuleCreateFunc)();

//组件辅助类模板
template <typename IModeluInterface> class CTempldateHelper
{
	//变量定义
protected:
	LPCSTR							m_pszFuncName;						//函数名字
	LPCTSTR							m_pszModuleDll;						//组件名字
	HINSTANCE						m_hDllInstance;						//DLL 句柄
	IModeluInterface				* m_pIModeluInterface;				//模块接口
	TCHAR							m_szErrorMsg[LEN_ERROR];			//错误信息

	//函数定义
public:
	//构造函数
	CTempldateHelper(LPCTSTR pszModuleDll, LPCSTR pszFuncName)
	{
		m_szErrorMsg[0]=0;
		m_hDllInstance=NULL;
		m_pIModeluInterface=NULL;
		m_pszFuncName=pszFuncName;
		m_pszModuleDll=pszModuleDll;
	}
	//析构函数
	virtual ~CTempldateHelper(void)
	{
		CloseInstance();
	}

	//功能函数
public:
	//创建函数
	bool CreateInstance()
	{
		CloseInstance();
		try
		{
			//加载 DLL
			if(m_hDllInstance == NULL)
				m_hDllInstance=LoadLibrary(m_pszModuleDll);
			if (m_hDllInstance==NULL) 
			{
#ifdef UNICODE
				_snwprintf(m_szErrorMsg,sizeof(m_szErrorMsg),TEXT("%s 加载失败, 错误代码:%ld!"),m_pszModuleDll,GetLastError());
#else
				_snprintf(m_szErrorMsg,sizeof(m_szErrorMsg),TEXT("%s 加载失败, 错误代码:%ld!"),m_pszModuleDll,GetLastError());
#endif
				return false;
			}

			//寻找导出函数
			ModuleCreateFunc * CreateFunc=(ModuleCreateFunc *)GetProcAddress(m_hDllInstance,m_pszFuncName);
			if (CreateFunc==NULL) 
			{
#ifdef UNICODE
				_snwprintf(m_szErrorMsg,sizeof(m_szErrorMsg),TEXT("%s 组件创建函数 %s 不存在, 错误代码:%ld!"),m_pszModuleDll,m_pszFuncName,GetLastError());
#else
				_snprintf(m_szErrorMsg,sizeof(m_szErrorMsg),TEXT("%s 组件创建函数 %s 不存在, 错误代码:%ld!"),m_pszModuleDll,m_pszFuncName,GetLastError());
#endif
				return false;
			}

			//创建组件
			m_pIModeluInterface=(IModeluInterface *)CreateFunc();
			if (m_pIModeluInterface==NULL) 
			{
#ifdef UNICODE
				_snwprintf(m_szErrorMsg,sizeof(m_szErrorMsg),TEXT("%s 组件创建函数 %s 创建组件失败!"),m_pszModuleDll,m_pszFuncName);
#else
				_snprintf(m_szErrorMsg,sizeof(m_szErrorMsg),TEXT("%s 组件创建函数 %s 创建组件失败!"),m_pszModuleDll,m_pszFuncName);
#endif
				return false;
			}
		}
		catch (...)	
		{ 
#ifdef UNICODE
			_snwprintf(m_szErrorMsg,sizeof(m_szErrorMsg),TEXT("%s 组件创建函数 %s 发生了异常错误!"),m_pszModuleDll,m_pszFuncName);
#else
			_snprintf(m_szErrorMsg,sizeof(m_szErrorMsg),TEXT("%s 组件创建函数 %s 发生了异常错误!"),m_pszModuleDll,m_pszFuncName);
#endif
			return false;
		}

		return true;
	}
	//关闭函数
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
	//是否有效
	bool IsValid()
	{ 
		if (m_pIModeluInterface==NULL) return false;
		return true;
	}
	//获取错误
	LPCTSTR GetErrorMessage() { return m_szErrorMsg; }

	//辅助函数
public:
	//指针重载
	IModeluInterface * operator->() {return GetInterface(); }
	//获取接口
	IModeluInterface * GetInterface() { return m_pIModeluInterface; }
};

// 此类有参数，用于检查DLL
typedef void * (ModuleCreateFuncCheck)(int iVer);

//组件辅助类模板
template <typename IModeluInterface> class CTempldateHelperCheck
{
	//变量定义
protected:
	LPCSTR							m_pszFuncName;						//函数名字
	LPCTSTR							m_pszModuleDll;						//组件名字
	HINSTANCE						m_hDllInstance;						//DLL 句柄
	IModeluInterface				* m_pIModeluInterface;				//模块接口
	TCHAR							m_szErrorMsg[LEN_ERROR];			//错误信息
	int							m_iVer;						// 版本
	//函数定义
public:
	//构造函数
	CTempldateHelperCheck(LPCTSTR pszModuleDll, LPCSTR pszFuncName, int iVer)
	{
		m_szErrorMsg[0]=0;
		m_hDllInstance=NULL;
		m_pIModeluInterface=NULL;
		m_pszFuncName=pszFuncName;
		m_pszModuleDll=pszModuleDll;
		m_iVer = iVer;
	}
	//析构函数
	virtual ~CTempldateHelperCheck(void)
	{
		CloseInstance();
	}

	//功能函数
public:
	//创建函数
	bool CreateInstance()
	{
		CloseInstance();
		try
		{
			//加载 DLL
			if(m_hDllInstance == NULL)
				m_hDllInstance=LoadLibrary(m_pszModuleDll);
			if (m_hDllInstance==NULL) 
			{
#ifdef UNICODE
				_snwprintf(m_szErrorMsg,sizeof(m_szErrorMsg),TEXT("%s 加载失败, 错误代码:%ld!"),m_pszModuleDll,GetLastError());
#else
				_snprintf(m_szErrorMsg,sizeof(m_szErrorMsg),TEXT("%s 加载失败, 错误代码:%ld!"),m_pszModuleDll,GetLastError());
#endif
				return false;
			}

			//寻找导出函数
			ModuleCreateFuncCheck * CreateFunc=(ModuleCreateFuncCheck *)GetProcAddress(m_hDllInstance,m_pszFuncName);
			if (CreateFunc==NULL) 
			{
#ifdef UNICODE
				_snwprintf(m_szErrorMsg,sizeof(m_szErrorMsg),TEXT("%s 组件创建函数 %s 不存在, 错误代码:%ld!"),m_pszModuleDll,m_pszFuncName,GetLastError());
#else
				_snprintf(m_szErrorMsg,sizeof(m_szErrorMsg),TEXT("%s 组件创建函数 %s 不存在, 错误代码:%ld!"),m_pszModuleDll,m_pszFuncName,GetLastError());
#endif
				return false;
			}

			//创建组件
			m_pIModeluInterface=(IModeluInterface *)CreateFunc(m_iVer);
			if (m_pIModeluInterface==NULL) 
			{
#ifdef UNICODE
				_snwprintf(m_szErrorMsg,sizeof(m_szErrorMsg),TEXT("%s 组件创建函数 %s 创建组件失败!"),m_pszModuleDll,m_pszFuncName);
#else
				_snprintf(m_szErrorMsg,sizeof(m_szErrorMsg),TEXT("%s 组件创建函数 %s 创建组件失败!"),m_pszModuleDll,m_pszFuncName);
#endif
				return false;
			}
		}
		catch (...)	
		{ 
#ifdef UNICODE
			_snwprintf(m_szErrorMsg,sizeof(m_szErrorMsg),TEXT("%s 组件创建函数 %s 发生了异常错误!"),m_pszModuleDll,m_pszFuncName);
#else
			_snprintf(m_szErrorMsg,sizeof(m_szErrorMsg),TEXT("%s 组件创建函数 %s 发生了异常错误!"),m_pszModuleDll,m_pszFuncName);
#endif
			return false;
		}

		return true;
	}
	//关闭函数
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
	//是否有效
	bool IsValid()
	{ 
		if (m_pIModeluInterface==NULL) return false;
		return true;
	}
	//获取错误
	LPCTSTR GetErrorMessage() { return m_szErrorMsg; }

	//辅助函数
public:
	//指针重载
	IModeluInterface * operator->() {return GetInterface(); }
	//获取接口
	IModeluInterface * GetInterface() { return m_pIModeluInterface; }
};
/////////////////////////////////////////////////////////////////////////////////////////

#endif