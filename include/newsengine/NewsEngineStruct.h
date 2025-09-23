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
// 导出类, 服务器状态
class  CNewsServerState
{
public:
	bool32		m_bConnected;			// 是否已连接上
	
	uint32		m_uiKeepSecond;			// 从连接上开始到现在维持时间（秒）
	uint32		m_uiSendBytes;			// 上行流量
	uint32		m_uiRecvBytes;			// 下行流量
	uint32		m_uiPingValue;			// ping值， 反应这台服务器的速度
	
	CString		m_StrServerName;		// 名字
	CString		m_StrServerAddr;		// Ip 地址
	
	int32		m_iCommunicationID;		// iCommunicationID
	int32		m_iServerID;			// ID
	uint32		m_uiServerPort;			// 端口
	uint32		m_uiServerHttpPort;		// HTTP端口
	
	int32		m_iOnLineCount;			// 在线数
	int32		m_iMaxCount;			// 最大在线数
	int32		m_iGroup;				// 所在组
	int32		m_iNetType;				// 服务器的网络类型
	
	uint32		m_iSortIndex;			// 排序的序号
	int32		m_iSortScore;			// 排序得分
};


enum E_NEWSSTAGE
{
	ENSTAGE_LoginInit = 0,		// 处在登录初始化场景中
	ENSTAGE_NewsSupplier,		// 新闻提供中


	ENSTAGE_COUNT
};

// 每笔业务应答记录
class AFX_EXT_CLASS CMmiNewsRespNode
{
public:
	CMmiNewsRespNode();
	~CMmiNewsRespNode();

public:
	// 记录下应答数据
	CMmiNewsBase	*m_pMmiNewsResp;

	//
	int32			m_iMmiRespId;			// 应答号

public:
	int32			m_iCommunicationId;		// 服务器id
	uint32			m_uiTimeResponse;		// 该服务器时间
};

// 导出类, 提供状态显示等服务器信息
class CAbsNewsCommnunicationShow
{
public:
	virtual void	GetNewsServerState(OUT CNewsServerState &ServerState) = 0;
	virtual void	GetNewsServerSummary(OUT bool32& bConnect, OUT CString& StrAddress, OUT uint32& uiPort, OUT uint32& uiKeepSeconds) = 0;
};

#endif 
