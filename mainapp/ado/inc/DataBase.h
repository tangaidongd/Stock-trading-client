#ifndef _DATA_BASE_H_
#define _DATA_BASE_H_

#pragma once

#include "Recordset.h"
#include <string>

using namespace std;

//////////////////////////////////////////////////////////////////////////

//数据库对象
class CDataBase
{
	friend CRecordSet;
	//信息变量
protected:
	CString							m_strConnect;						//连接字符串
	CString							m_strErrorDescribe;					//错误信息
	
	//状态变量
protected:
	DWORD							m_dwConnectCount;					//重试次数
	DWORD							m_dwConnectErrorTime;				//错误时间
	const DWORD						m_dwResumeConnectCount;				//恢复次数
	const DWORD						m_dwResumeConnectTime;				//恢复时间
	
	//内核变量
protected:
	_CommandPtr						m_DBCommand;						//命令对象
	_ConnectionPtr					m_DBConnection;						//数据库对象
	
	//函数定义
public:
	//构造函数
	CDataBase();
	//析构函数
	virtual ~CDataBase();
	
	//管理接口
public:
	//打开连接
	virtual bool __cdecl OpenConnection();
	
	//关闭连接
	virtual bool __cdecl CloseConnection();
	//重新连接
	virtual bool __cdecl TryConnectAgain(bool bFocusConnect, CComError * pComError);
	//设置信息
	virtual bool __cdecl SetConnectionInfo(LPCTSTR szIP,LPCTSTR szPipeName, WORD wPort, LPCTSTR szData, LPCTSTR szName, LPCTSTR szPass);
	
	//状态接口
public:
	//是否连接错误
	virtual bool __cdecl IsConnectError();
	
	//命令对象接口
public:
	//设置存储过程
	virtual void __cdecl SetSPName(LPCTSTR pszSpName);
	//插入参数
	virtual void __cdecl AddParamter(LPCTSTR pszName, ADOCG::ParameterDirectionEnum Direction, ADOCG::DataTypeEnum Type, long lSize, const _variant_t & vtValue);
	//插入参数
	virtual void __cdecl AddDecimalParamter(LPCTSTR pszName, ADOCG::ParameterDirectionEnum Direction, DOUBLE& dbValue, 
		unsigned char Precision = 24, unsigned char NumericScale = 4);
	//插入参数
	virtual void __cdecl AddDecimalParamter(LPCTSTR pszName, ADOCG::ParameterDirectionEnum Direction, DECIMAL& decValue, 
		unsigned char Precision = 24, unsigned char NumericScale = 4);
	//删除参数
	virtual void __cdecl ClearAllParameters();
	//获得参数
	virtual void __cdecl GetParameterValue(LPCTSTR pszParamName, _variant_t & vtValue);
	//获取返回数值
	virtual long __cdecl GetReturnValue();
	
	//执行接口
public:
	//执行语句
	virtual bool __cdecl Execute(LPCTSTR pszCommand, IN OUT CRecordSet *pRecordSet);
	//执行命令
	virtual bool __cdecl ExecuteCommand(IN OUT CRecordSet *pRecordSet);
	//写入大数据
	virtual bool __cdecl WriteBinary(IN LPCTSTR lpSelect, IN LPCTSTR lpFieldName, IN const char *pBinaryData, IN int iDataLen);
	// 初始化COM环境
	static bool InitComEnv();
	
	static void UnitComEnv();
	//获取错误
	LPCTSTR GetComErrorDescribe();
	//内部函数
	
	_ConnectionPtr GetConnection();
private:
	
	//设置错误
	void SetErrorInfo(CComError & ComError);
};
#endif