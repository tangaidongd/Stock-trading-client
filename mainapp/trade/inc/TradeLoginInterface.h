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
	ETT_TRADE_FIRM = 0,	// ʵ�̽���
	ETT_TRADE_SIMULATE,	// ģ�⽻��
};

struct T_CommInfoType 
{
	int iIndex;
	char chTypeName[64];	// ��ʾ����
	char chTypeValue[64];	// ����ֵ
	
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

	CString		StrLoginUser;   // ��¼�û���
	CString		StrUser;		// ��¼�󷵻ص��ʺ�
	CString		StrPwd;
//	CString		StrValidCode;	// ��֤��
//	CString		StrUuid;		// ��֤�뷵����Ϣ 

	int32		StrSessionID;	// SessionID
	CString		StrUserSessionID;	// ���Ժ�ÿ����������д
	CString		StrUserSessionInfo;	// ���Ժ�ÿ����������д ���������ڶ�Ԫ�����̽���

	int32		ThreadID;			// �߳�id ע����������ʱ���� _Head �� uReserve���ظ�������	

//	ServerObj serverobj;		// ��������Ϣ
	T_ProxyInfo m_proxyInfol;   // ����

	E_TradeLoginType eTradeLoginType;	// ��������
	CString		StrQuoteFlag;	// "0"�ṩ�������飬 "1"���ṩ��������
	CString		StrMarketType;	// �г����� ���quote_flagʹ��
};

// struct T_EnrustParameters
// {
// 	OrderShiJiaIn orderMarket;
// 	OrderZhiJiaIn orderLimit;
// 	int iTradeType;	// 0-�м� 1-ָ��
// };

class iTradeLoginNotify;
class iTradeLogin
{
public:
	virtual bool    Connect(const T_TradeLoginInfo &stLoginInfo, iTradeLoginNotify *pNotify) = 0;
	virtual void	DisConnect(int iMode=0) = 0;
	virtual void	StopLogin(int iMode=0) = 0;
	virtual void	OnLoginDlgCancel() = 0;		// ��¼����Ҫ�ر���
};

class iTradeLoginNotify
{
public:
	virtual void	OnLoginOK() = 0;
	virtual void	OnLoginUserStop() = 0;			// ��¼��ֹ��
	virtual void	OnLoginError(const CString &StrErr) = 0;
	virtual void	OnLoginStep(const CString &Str) = 0;		// ��½ʱ�Ĳ�����ʾ	
	virtual void	OnUpdateValidCode() = 0;		// ��¼ʧ�ܸ�����֤��
};

extern const char*	  KStrElementAttriVesion;

#endif //!_TRADELOGININTERFACE_H_