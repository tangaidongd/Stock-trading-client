// NewsManager.h: interface for the CNewsManager class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _NEWS_ENGINE_MANAGER_H
#define _NEWS_ENGINE_MANAGER_H

#include "NewsEngineStruct.h"


// 通知类
class AFX_EXT_CLASS CAbsCNewsManagerNotify
{
public:
	virtual void	OnNewsResponse() = 0;
	virtual void	OnNewsServerConnected(int32 iCommunicationId) = 0;
	virtual void	OnNewsServerDisconnected(int32 iCommunicationId) = 0;
};


// 导出类, 提供对外的接口
class AFX_EXT_CLASS CAbsNewsManager
{
public:
	// 初始化
	virtual void	InitAllNewsCommunication(IN const CProxyInfo &ProxyInfo, IN const CArray<T_ServerInfo, T_ServerInfo&>& aNewsServers, IN const CString &StrUserName, IN const CString &StrPassword) = 0;

	// 启动所有资讯服务器
	virtual	bool32	StartAllNewsCommunication(bool32 bOnlyFirst = false) = 0;

	// 停止所有资讯服务器
	virtual bool32  StopAllNewsCommunication() = 0;

	// 请求数据
	virtual bool32	RequestNews(IN CMmiNewsBase* pMmiNewsBase, IN int32 iCommunicationId = -1) = 0;

	// 取应答包
	virtual CMmiNewsRespNode* PeekAndRemoveResp() = 0;

	// 设置通知对象
	virtual void SetNewsManagerNotifyObj(CAbsCNewsManagerNotify* pNotify) = 0;

	//释放内存空间
	virtual void Release() = 0;

	//设置当前场景信息
	virtual void SetStage(E_NEWSSTAGE eStage) = 0;
	
	// 取服务器信息
	virtual void	GetCommnunicaionList(OUT CArray<CAbsNewsCommnunicationShow*, CAbsNewsCommnunicationShow*>& aServers) = 0;
};

AFX_EXT_CLASS CAbsNewsManager * GetNewsEngineObj();

#endif // !defined(_NEWS_ENGINE_MANAGER_H)
