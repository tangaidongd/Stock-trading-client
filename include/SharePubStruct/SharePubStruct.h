#ifndef _SHARE_FUN_STRUCT_H_
#define _SHARE_FUN_STRUCT_H_
#define  MAX_NAME_LEN	64

enum E_ServerType
{
	enSTNone	= 0,
	enSTQuote	= 1, 	// 行情服务器
	enSTTrade	= 2, 	// 交易服务器
	enSTNews	= 4,	// 消息服务器
	enSTWarn	= 8,	// 预警
	enSTInfo    = 32,   // 资讯服务器
};

// 公共结构体
typedef struct _ServerInfo
{
	int				iID;						// 服务器ID
	E_ServerType	enType;						// 服务器类型
	wchar_t			wszName[MAX_NAME_LEN];			// 名字
	wchar_t			wszAddr[32];				// IP
	int				iPort;						// 端口
	int				iHttpPort;					// HTTP端口
	int				iOnLineCount;				// 在线数
	int				iMaxCount;					// 最大在线数
	int				iSerNetID;					// 服务器的网络类型ID
	wchar_t			wszSerKey[MAX_NAME_LEN];

	int				iGroup;						// 所在组

	_ServerInfo()
	{
		iID					= 0;
		enType				= enSTNone;
		wszName[0]			= 0;
		wszAddr[0]			= 0;
		wszSerKey[0]		= 0;
		iPort				= 0;
		iHttpPort			= 0;
		iOnLineCount		= 0;
		iMaxCount			= 0;
		iSerNetID			= 0;
		iGroup				= 1;
	};

	_ServerInfo(const _ServerInfo& serverinfo)
	{
		iID			= serverinfo.iID;
		enType		= serverinfo.enType;
		memcpy(wszName, serverinfo.wszName, sizeof(wchar_t)*MAX_NAME_LEN);
		memcpy(wszAddr, serverinfo.wszAddr, sizeof(wchar_t)*32);
		memcpy(wszSerKey, serverinfo.wszSerKey, sizeof(wchar_t)*MAX_NAME_LEN);
		iPort		= serverinfo.iPort;
		iHttpPort	= serverinfo.iHttpPort;
		iOnLineCount = serverinfo.iOnLineCount;
		iMaxCount	= serverinfo.iMaxCount;
		iSerNetID	= serverinfo.iSerNetID;
		iGroup		= serverinfo.iGroup;	
	}

	_ServerInfo& operator=(const _ServerInfo& serverinfo)//重载运算符  
	{  
		if ( this == &serverinfo )
		{
			return *this;
		}

		iID			= serverinfo.iID;
		enType		= serverinfo.enType;
		memcpy(wszName, serverinfo.wszName, sizeof(wchar_t)*MAX_NAME_LEN);
		memcpy(wszAddr, serverinfo.wszAddr, sizeof(wchar_t)*32);
		memcpy(wszSerKey, serverinfo.wszSerKey, sizeof(wchar_t)*MAX_NAME_LEN);
		iPort		= serverinfo.iPort;
		iHttpPort	= serverinfo.iHttpPort;
		iOnLineCount = serverinfo.iOnLineCount;
		iMaxCount	= serverinfo.iMaxCount;
		iSerNetID	= serverinfo.iSerNetID;
		iGroup		= serverinfo.iGroup;

		return *this;
	} 

}T_ServerInfo;

#endif