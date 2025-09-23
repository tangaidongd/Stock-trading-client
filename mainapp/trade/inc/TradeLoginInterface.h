#ifndef _TRADELOGININTERFACE_H_
#define _TRADELOGININTERFACE_H_

#include "LoginUserData.h"

#include <vector>
using std::vector;
#include <set>
using std::set;
#include <map>
using std::map;

enum E_EntrustType
{
	EET_NONE = 0,
	EET_BUY,
	EET_SELL,
	EET_END
};

enum E_OpenCloseType
{
	EOCT_NONE = 0,
	EOCT_OPEN,
	EOCT_CLOSE,
};

enum E_TradeLoginType
{
	ETT_TRADE_FIRM = 0,	// 实盘交易
	ETT_TRADE_SIMULATE,	// 模拟交易
};

struct T_CommInfoType 
{
	int iIndex;
	char chTypeName[64];	// 显示名称
	char chTypeValue[64];	// 请求值
	
	T_CommInfoType()
	{
		iIndex = 0;
		chTypeName[0]=0;
		chTypeValue[0]=0;
	}
};
typedef std::map<int, T_CommInfoType>	_CommInfoType;

struct T_TradeLoginInfo 
{
	CString		StrServerName;
	CString		StrServerIP;
	int			iServerPort;

	CString		StrLoginUser;   // 登录用户名
	CString		StrUser;		// 登录后返回的帐号
	CString		StrPwd;
//	CString		StrValidCode;	// 验证码
//	CString		StrUuid;		// 验证码返回信息 

	int32		StrSessionID;	// SessionID
	CString		StrUserSessionID;	// 在以后每个请求中填写
	CString		StrUserSessionInfo;	// 在以后每个请求中填写 仅适用于于多元做市商交易

	int32		ThreadID;			// 线程id 注：其他请求时放在 _Head 中 uReserve返回给服务器	

//	ServerObj serverobj;		// 服务器信息
	T_ProxyInfo m_proxyInfol;   // 代理

	E_TradeLoginType eTradeLoginType;	// 交易类型
	CString		StrQuoteFlag;	// "0"提供交易行情， "1"不提供交易行情
	CString		StrMarketType;	// 市场代码 配合quote_flag使用
};

// struct T_EnrustParameters
// {
// 	OrderShiJiaIn orderMarket;
// 	OrderZhiJiaIn orderLimit;
// 	int iTradeType;	// 0-市价 1-指价
// };

class iTradeLoginNotify;
class iTradeLogin
{
public:
	virtual bool    Connect(const T_TradeLoginInfo &stLoginInfo, iTradeLoginNotify *pNotify) = 0;
	virtual void	DisConnect(int iMode=0) = 0;
	virtual void	StopLogin(int iMode=0) = 0;
	virtual void	OnLoginDlgCancel() = 0;		// 登录界面要关闭了
};

class iTradeLoginNotify
{
public:
	virtual void	OnLoginOK() = 0;
	virtual void	OnLoginUserStop() = 0;			// 登录终止了
	virtual void	OnLoginError(const CString &StrErr) = 0;
	virtual void	OnLoginStep(const CString &Str) = 0;		// 登陆时的步骤显示	
	virtual void	OnUpdateValidCode() = 0;		// 登录失败更新验证码
};

extern const char*	  KStrElementAttriVesion;

#endif //!_TRADELOGININTERFACE_H_