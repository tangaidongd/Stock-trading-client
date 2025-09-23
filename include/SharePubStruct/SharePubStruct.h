#ifndef _SHARE_FUN_STRUCT_H_
#define _SHARE_FUN_STRUCT_H_
#define  MAX_NAME_LEN	64

enum E_ServerType
{
	enSTNone	= 0,
	enSTQuote	= 1, 	// ���������
	enSTTrade	= 2, 	// ���׷�����
	enSTNews	= 4,	// ��Ϣ������
	enSTWarn	= 8,	// Ԥ��
	enSTInfo    = 32,   // ��Ѷ������
};

// �����ṹ��
typedef struct _ServerInfo
{
	int				iID;						// ������ID
	E_ServerType	enType;						// ����������
	wchar_t			wszName[MAX_NAME_LEN];			// ����
	wchar_t			wszAddr[32];				// IP
	int				iPort;						// �˿�
	int				iHttpPort;					// HTTP�˿�
	int				iOnLineCount;				// ������
	int				iMaxCount;					// ���������
	int				iSerNetID;					// ����������������ID
	wchar_t			wszSerKey[MAX_NAME_LEN];

	int				iGroup;						// ������

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

	_ServerInfo& operator=(const _ServerInfo& serverinfo)//���������  
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