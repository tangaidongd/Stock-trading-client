#ifndef _LOGINUSERDATA_H_
#define _LOGINUSERDATA_H_


// 服务器配置
extern const char*	  KStrElementNameServerInfoQuote;
extern const char*	  KStrElementAttriServerIndex;
extern const char*	  KStrElementNameServer;
extern const char*	  KStrElementAttriServerName;
extern const char*	  KStrElementAttriServerIp;
extern const char*	  KStrElementAttriServerPort;
extern const char*	  KStrElementAttriServerNetType;

extern const char*	  KStrElementNameBackupServerPort;		//备用行情端口

//extern const char*	  KStrElementAttriServerDefault;
//extern const char*	  KStrElementNameServerInfoNews;
//extern const char*	  KStrElementNameServerInfoOrgCode;
//extern const char*	  KStrElementNameServerInfoNewsOrgCode;
//extern const char*	  KStrElementNameServerInfoPlatCode;


extern const char*    KStrElementAttriPickModelServer;
extern const char*	  KStrElementAttriPickModelType;

extern const char*	  KStrElementNameServerInfoTrades;
extern const char*	  KStrElementAttriServerTradeIndex;
extern const char*    KStrElementAttriServerInfoCenter;
extern const char*    KStrElementAttriGateWayInfo;
extern const char*    KStrElementAttriQueryStockInfo;
extern const char*	  KStrElementnNameServerEcono;
extern const char*    KStrElementAttriServerEconoURL;
// 用户配置
extern const char*	  KStrElementNameUserInfo;
extern const char*	  KStrElementNameUser;
extern const char*	  KStrElementAttriUserName;
extern const char*	  KStrElementAttriUserPwd;
extern const char*	  KStrElementAttriBeSave;
extern const char*    KStrElementAttriBeLastUser;
extern const char*    KStrElementAttriEnvIndex;
extern const char*	  KStrElementAttriAutoLogin;
extern const char*	  KStrElementAttriSCToolBar;

// 代理配置
extern const char*	  KStrElementNameProxyInfo;
extern const char*	  KStrElementNameProxy;
extern const char*	  KStrElementAttriBeUseProxy;
extern const char*	  KStrElementAttriProxyType;
extern const char*	  KStrElementAttriProxyAddress;
extern const char*	  KStrElementAttriProxyPort;
extern const char*	  KStrElementAttriProxyUserName;
extern const char*	  KStrElementAttriProxyUserPwd;

// 权限
extern const char*	  KStrElementNameUserRight;
extern const char*	  KStrElementAttriRightCode;
extern const char*	  KStrElementAttriRightName;

//网络设置
extern const char*   KStrElementAttriNetWorkSet;
extern const char*   KStrElementAttriNetWork;
extern const char*   KStrElementAttriNetWorkName;
extern const char*   KStrElementAttriNetWorkURL;
extern const char*   KStrElementAttriNetWorkPort;
extern const char*   KStrElementAttriNetWorkSelected;
extern const char*   KStrElementAttriNetWorkRunMode;
extern const char*   KStrElementAttriSimulateTrade;

extern const char*	 KStrElementAttriGreenChannel;

typedef struct T_HostInfo 
{
	CString   StrHostName;
	CString	  StrHostAddress;
	UINT	  uiHostPort;
	UINT	  uiNetType;

	T_HostInfo(){
		uiHostPort = 0;
		uiNetType = 0;
	}
}T_HostInfo;

typedef struct T_PlusInfo
{	
	CString strName;
	CString strUrl;
	int iPort;
	int  iSelected;
}T_PlusInfo;

typedef struct T_ProxyInfo 
{
	UINT	  uiProxyType;
	CString   StrProxyAddress;
	UINT	  uiProxyPort;
	CString	  StrUserName;
	CString	  StrUserPwd;
}T_ProxyInfo;

typedef struct T_TradeUserInfo
{
	CString		StrUserName;
	CString		StrPassword;		//这里存在安全隐患
	bool32		BeSavePassword;
	bool32		BeLastUser;
	int32		IndexServer;
	T_TradeUserInfo()
	{
		BeSavePassword = FALSE;
		BeLastUser = FALSE;
		IndexServer = 0;
	}
}T_TradeUserInfo;

class CServerConfigManager
{
public:
	//
	typedef CArray<T_HostInfo, const T_HostInfo &> HostInfoArray;
	//
	~CServerConfigManager(){};

	static CServerConfigManager &Instance();

	bool32	ToXml();	// TODO
	bool32	FromXml();

	bool32	GetTradeServers(OUT HostInfoArray &aServers);
	bool32	GetQuoteServers(OUT HostInfoArray &aServers);
	bool32	GetNewsServers(OUT HostInfoArray &aServers);

private:
	HostInfoArray	m_aTrades;
	HostInfoArray	m_aQuotes;
	HostInfoArray	m_aNews;

private:
	CServerConfigManager();
	CServerConfigManager(const CServerConfigManager &);
	const CServerConfigManager &operator=(const CServerConfigManager &);
};


#endif //_LOGINUSERDATA_H_