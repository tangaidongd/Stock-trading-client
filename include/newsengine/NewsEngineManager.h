// NewsManager.h: interface for the CNewsManager class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _NEWS_ENGINE_MANAGER_H
#define _NEWS_ENGINE_MANAGER_H

#include "NewsEngineStruct.h"


// ֪ͨ��
class AFX_EXT_CLASS CAbsCNewsManagerNotify
{
public:
	virtual void	OnNewsResponse() = 0;
	virtual void	OnNewsServerConnected(int32 iCommunicationId) = 0;
	virtual void	OnNewsServerDisconnected(int32 iCommunicationId) = 0;
};


// ������, �ṩ����Ľӿ�
class AFX_EXT_CLASS CAbsNewsManager
{
public:
	// ��ʼ��
	virtual void	InitAllNewsCommunication(IN const CProxyInfo &ProxyInfo, IN const CArray<T_ServerInfo, T_ServerInfo&>& aNewsServers, IN const CString &StrUserName, IN const CString &StrPassword) = 0;

	// ����������Ѷ������
	virtual	bool32	StartAllNewsCommunication(bool32 bOnlyFirst = false) = 0;

	// ֹͣ������Ѷ������
	virtual bool32  StopAllNewsCommunication() = 0;

	// ��������
	virtual bool32	RequestNews(IN CMmiNewsBase* pMmiNewsBase, IN int32 iCommunicationId = -1) = 0;

	// ȡӦ���
	virtual CMmiNewsRespNode* PeekAndRemoveResp() = 0;

	// ����֪ͨ����
	virtual void SetNewsManagerNotifyObj(CAbsCNewsManagerNotify* pNotify) = 0;

	//�ͷ��ڴ�ռ�
	virtual void Release() = 0;

	//���õ�ǰ������Ϣ
	virtual void SetStage(E_NEWSSTAGE eStage) = 0;
	
	// ȡ��������Ϣ
	virtual void	GetCommnunicaionList(OUT CArray<CAbsNewsCommnunicationShow*, CAbsNewsCommnunicationShow*>& aServers) = 0;
};

AFX_EXT_CLASS CAbsNewsManager * GetNewsEngineObj();

#endif // !defined(_NEWS_ENGINE_MANAGER_H)
