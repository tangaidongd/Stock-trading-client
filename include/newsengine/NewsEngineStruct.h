// NewsCommnunication.h: interface for the CNewsCommnunication class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _NEWS_ENGINE_STRUCT_H
#define _NEWS_ENGINE_STRUCT_H


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "typedef.h"
#include "SocketClient.h"
#include "sharestructnews.h"
#include "synch.h"
#include "SharePubStruct.h"


#define  MAX_NEWS_NAME_LEN	64
// ������, ������״̬
class  CNewsServerState
{
public:
	bool32		m_bConnected;			// �Ƿ���������
	
	uint32		m_uiKeepSecond;			// �������Ͽ�ʼ������ά��ʱ�䣨�룩
	uint32		m_uiSendBytes;			// ��������
	uint32		m_uiRecvBytes;			// ��������
	uint32		m_uiPingValue;			// pingֵ�� ��Ӧ��̨���������ٶ�
	
	CString		m_StrServerName;		// ����
	CString		m_StrServerAddr;		// Ip ��ַ
	
	int32		m_iCommunicationID;		// iCommunicationID
	int32		m_iServerID;			// ID
	uint32		m_uiServerPort;			// �˿�
	uint32		m_uiServerHttpPort;		// HTTP�˿�
	
	int32		m_iOnLineCount;			// ������
	int32		m_iMaxCount;			// ���������
	int32		m_iGroup;				// ������
	int32		m_iNetType;				// ����������������
	
	uint32		m_iSortIndex;			// ��������
	int32		m_iSortScore;			// ����÷�
};


enum E_NEWSSTAGE
{
	ENSTAGE_LoginInit = 0,		// ���ڵ�¼��ʼ��������
	ENSTAGE_NewsSupplier,		// �����ṩ��


	ENSTAGE_COUNT
};

// ÿ��ҵ��Ӧ���¼
class AFX_EXT_CLASS CMmiNewsRespNode
{
public:
	CMmiNewsRespNode();
	~CMmiNewsRespNode();

public:
	// ��¼��Ӧ������
	CMmiNewsBase	*m_pMmiNewsResp;

	//
	int32			m_iMmiRespId;			// Ӧ���

public:
	int32			m_iCommunicationId;		// ������id
	uint32			m_uiTimeResponse;		// �÷�����ʱ��
};

// ������, �ṩ״̬��ʾ�ȷ�������Ϣ
class CAbsNewsCommnunicationShow
{
public:
	virtual void	GetNewsServerState(OUT CNewsServerState &ServerState) = 0;
	virtual void	GetNewsServerSummary(OUT bool32& bConnect, OUT CString& StrAddress, OUT uint32& uiPort, OUT uint32& uiKeepSeconds) = 0;
};

#endif 
