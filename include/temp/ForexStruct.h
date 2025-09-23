#include <afxtempl.h>
#include <string>

using namespace std;

#ifndef _FOREX_STRUCT_H_
#define _FOREX_STRUCT_H_

// 协议类型
typedef enum E_ForexInfo
{
	EFIReqOrder				= 7001,			// 请求下单(数据结构等于 9513)
	EFIRespOrder			= 7002,			// 下单回包(数据结构等于 9513)
	EFIRespBargain			= 7003,			// 即时单交易成功(数据结构等于 9512)
	EFIRespModifyEntry		= 7004,			// 修改挂单(数据结构等于 9513)
	EFIRespDelEntry			= 7005,			// 删除挂单
	EFIReqHistoryOrder		= 7900,			// 查询历史成交记录

	EFILogin				= 8000,			// 登录
	EFILogOut				= 8001,			// 登出
	EFIHeart				= 9002,			// 心跳
	EFIAccount				= 9501,			// 帐户信息
	EFICurrency				= 9505,			// 货币信息
	EFIDefaultMerch			= 9511,			// 默认商品信息
	EFIBargain				= 9512,			// 成交信息
	EFIOrder				= 9513,			// 委托单信息
	EFIHistoryOrder			= 9514,			// 结算单信息
	EFIGroup				= 9515,			// 组信息
	EFIBase					= 9516,			// 基本信息
	EFIQuotePush			= 9518,			// 行情推送
	EFIBroadcast			= 9524,			// 公告信息
	EFIError				= 9999,			// 错误消息

}E_ForexReqType;


// 错误类型
enum E_ErrorCode
{
	// 自定义的错误
	ErrNone			= 0,	// 没有错误
	ErrInitalFailed,		// 初始化失败
	ErrSslIniFailed,		// SSL 初始化失败
	ErrAddressInValid,		// 地址信息错误
	ErrConnectFailed,		// connect 失败
	ErrSslConnectFailed,	// SSL connect 失败
	ErrThreadFailed,		// Thread Create Failed
	
	// 服务器返回的错误
	Err1001 = 1001,			//	数量不正确                                  
	Err1002,                //	价格不正确                                  
	Err1003,                //	未知错误                                    
	Err1004,                //	代码不存在                                  
	Err1005,                //	密码错误                                    
	Err1006,                //	相同帐号从其它终端登录，您已被强制退出系统  
	Err1007,                //	无操作权限                                  
	Err1008,                //	非交易时间                                  
	Err1009,                //	资金不足                                    
	Err1010,                //	产品不正确                                  
	Err1011,                //	委托条件不正                                
	Err1012,                //	委托已被撤单                                
	Err1013,                //	委托已被完全                                
	Err1014,                //	委托已被撤单                                
	Err1015,                //	委托已成交，                                
	Err1016,                //	指示已经无效                                
	Err1017,                //	委托已被完全                                
	Err1018,                //	委托已被撤单                                
	Err1019,                //	委托已是生效                                
	Err1020,                //	委托已成交，                                
	Err1021,                //	委托已被撤单                                
	Err1022,                //	密码错误                                    
	Err1023,                //	新密码太短                                  
	Err1024,                //	代码不能为空                                
	Err1025,                //	客户代码长度                                
	Err1026,                //	密码长度必须                                
	Err1027,                //	前后输入密码                                
	Err1028,                //	信贷额必须为                                
	Err1029,                //	无效操作指令                                
	Err1030,                //	交易服务断开                                
	Err1031,                //	帐号被冻结                                  
	Err1032,                //	止损价格不正确                              
	Err1033,                //	获利价格不正确                              
	Err1034,                //	商品代码不存在                              
	Err1035,                //	可平仓数量不正确                            
	Err1036,                //	非交易品种                                  
	Err1037,                //	市场价格已改变                              
	Err1038,                //	要平仓的单不存在                            
	Err1039,                //	要操作的委托单不存在                        
	Err1040,                //	金额不正确                                  
	Err1041,                //	未知货币                                    
	Err1042,                //	登录帐号不存在                              
	Err1043,                //	请检查网络连接                              
	Err1044,                //	平仓间隔时间短于规定值                      
	Err1045,                //	禁止反向新单                                
	Err1046,                //	有持仓时禁止修改交收额/信用额               
	Err1047,                //	交收额不能大于信用额                        
	Err1048,                //	信用额数值不合法                            
	Err1049,                //	可取金额不足                                
	Err1050,                //	数量超过持仓限额                            
	Err1051,                //	系统繁忙,                //请稍后再行操作                     
	Err1052,                //	止损价格超出指定范围，请参考帮助            
	Err1053,                //	止赢价格超出指定范围，请参考帮助            
	Err1054,                //	挂价不合法                                  
	Err1055,                //	行情不同步,                //等待重新报价                     
	Err3001 = 3001,                //	记录已存在                                  
	Err3002,                //	关键字段不能为空                            
	Err3003,                //	市场代码不对                                
	Err3004,                //	货币代码不对                                
	Err3005,                //	保证金类型不对                              
	Err3006,                //	利息类型不对                                
	Err3007,                //	手续费编码不对                              
	Err3008,                //	外汇类别不对                                
	Err3009,                //	委托已存在                                  
	Err3010,                //	查询记录不存在                              
	Err5001 = 5001,                //	同步调用超时                                
	Err5002,                //	寻找主机错误，未找到主机                    
	Err5003,                //	寻找主机错误，未找到主机                    
	Err6001 = 6001,                //	修改密码时新密码不一致                      
	Err6002,                //	密码长度必须在6到16位之间                   
};

// 实时行情
class CForexRealtimePrice
{
public:
	CForexRealtimePrice(){}
	~CForexRealtimePrice(){}

	//
	CString		m_StrCode;
	float		m_fBuy;
	float		m_fSell;
	float		m_fHigh;
	float		m_fLow;
	CString		m_StrUpdateTime;
};

// 服务器信息
class CServer
{
public:
	CString		m_StrServerAdd;
	DWORD		m_dwPort;
};

// 外汇商品类
class CForexMerch  
{
public:
	CForexMerch() {}
	virtual ~CForexMerch(){}
 
public:
	CString			m_StrCode;			// 1	代码
	int				m_iMerchType;		// 2	商品类型 9:Exoit,10:Major,11:Minor,12:Metal,13:差价合约,14:指数
	CString			m_StrUnit;			// 3	单位, 一直没有值, 不知道是什么类型的字段 ...fangz 0720
	int				m_iChargeType;		// 4	手续费类型
	int				m_iDepositType;		// 5	保证金类型
	int				m_iInterestType;	// 6	利息类型
	float			m_fContractSize;	// 7	合约大小
	float			m_fDepositValue;	// 8	保证金值
	float			m_fDepositNight;	// 9	过夜保证金值
	float			m_fMinPriceChange;	// 10	最小价格变动
	float			m_fBuyInterest;		// 11	买入利息值	
	float			m_fSellInterest;	// 12	卖出利息值	
    float			m_fChargeOpen;		// 13	开仓手续费	
	float			m_fChargeClose;		// 14	平仓手续费	
	float			m_fPointDiff;		// 15	点差	
	float			m_fPriceLmtPtDiff;	// 16	限价点差	
	int				m_iFlagTrade;		// 17	允许交易标志	
	float			m_fTimesPrice;		// 18	价格乘数	实际价格=价格/价格乘数
	float			m_fTimesVolume;		// 19	数量乘数	实际数量数量/数量乘数
	float			m_fMinTradeVol;		// 20	最小交易数量	
	int				m_iMerchType1;		// 21	商品类型1	1:以美元作为报价货币,2:以美元作为基本货币,3:交叉盘，汇率对以美元作为报价货币,4:交叉盘，汇率对以美元作为基本货币 ...fangz 0720 两个商品类型??
	float			m_fPriceBuy;		// 22	买入价格	
	float			m_fPriceSell;		// 23	卖出价格	
	float			m_fPriceHigh;		// 24	最高价格	
	float			m_fPriceLow;		// 25	最低价格	
	float			m_fPriceOpen;		// 26	开盘价格	
	float			m_fPricePreClose;	// 27	上日收市价格	
	float			m_fReserver29;		// 28	[备用字段]	
	//float			m_fReserver30;		// 29	[备用字段]	
	CString			m_StrDataSourceMerch;	// 29 数据来源商品(天河交易特有)
	float			m_iMarketID;		// 30	市场代码	
	float			m_fReserver32;		// 31	[备用字段]	
	CString			m_Strcurrency;		// 32	货币	
	int				m_iTradeType;		// 33	交易类型	
	float			m_fMaxMarketVol;	// 34	最大市场成交数量	
	int				m_iValidBillType;	// 35	挂单有效期类型	91：一直有效，92：日内有效（包括止赢止损）,93:日内有效(除止赢止损)，110：挂单本日，止赢止损本周，111：挂单本周，止赢止损本日
	int				m_i3DaysCalcType;	// 36	三日计息模式	120:周日,121:周一,122:周二,123:周三,124:周四,125周五,126:周六
	float			m_afReserver[16];	// 37-52	[备用字段]	
	float			m_iLockDepositType;	// 53	锁单保证金类型	
	float			m_fReserver55;		// 54	[备用字段]	
	int				m_iFlagDefault;		// 55	缺省标志	
	CString			m_StrOverDate;		// 56	到期日	
	float			m_fDepositAlarm;	// 57	保证金报警水平	
	float			m_fForceClose;		// 58	强制平仓水平	
	float			m_fReserver60;		// 59	[备用字段]	
	float			m_fBillCharge;		// 60	仓租	
	//float			m_afReserver2[26];	// 61-86 [备用字段]	
	float			m_afReserver2[14];	// 61-74 [备用字段]	
	int				m_iOrderPriceMaxType;	// 75 挂单上限类型
	int				m_iOrderPriceMinType;	// 76 挂单下限类型
	int				m_iStopLoseMaxType;		// 77 止损上限类型
	int				m_iStopLoseMinType;		// 78 止损下限类型
	int				m_iStopProfitMaxType;	// 79 止盈上限类型
	int				m_iStopProfitMinType;	// 80 止盈下限类型
	float			m_fOrderPriceMax;		// 81 挂单上限值
	float			m_fOrderPriceMin;		// 82 挂单下限值
	float			m_fStopLostMax;			// 83 止损上限值
	float			m_fStopLoseMin;			// 84 止损下限值
	float			m_fStopProfitMax;		// 85 止盈上限值
	float			m_fStopProfitMin;		// 86 止盈下限值
	float			m_fLockDepositValue;// 87	锁单保证金值
	float			m_fReserve88;		// 88	[备用字段]
	//
	CForexRealtimePrice	m_RealtimePrice;// 实时行情数据, 9518 推送	
};

// 货币信息
class CCurrency
{
public:

	CCurrency(){}
	~CCurrency(){}

public:

	CString		m_StrCode;			// 货币代码
	CString		m_StrName;			// 货币名称	
	CString		m_StrNameCn;		// 中文名称
	float		m_fInterestRate;	// 利率
	float		m_fExchangeRate;	// 兑结算货币汇率
	int			m_iFlagCurrency;	// 结算货币标志
	float		m_fReserver7;		// 保留字段
	float		m_fReserver8;		// 保留字段
};	

// 组信息
class CGroup
{
public:

	CGroup(){}
	~CGroup(){}

public:
	CString		m_StrCode;				// 组代码
	int			m_iState;				// 状态, 42正常，43冻结，44只读，45已销户
	CString		m_StrCompanyName;		// 公司名称
	CString		m_StrDepositCurrency;	// 保证金货币
	int			m_iTimesDefault;		// 缺省倍数
	float		m_fIncome;				// 投资收益
	int			m_iHighSecurityFlag;	// 高安全级别标志
	float		m_afReserver[27];		// 剩下 27 个保留字段
};

// 公告信息
class CBroadCast
{
public:

	CBroadCast(){}
	~CBroadCast(){}

public:
	int			m_iNo;					// 信息编号
	float		m_fReserver2;			// 保留字段
	float		m_fReserver3;			// 保留字段
	CString		m_StrTitle;				// 标题
	CString		m_StrAuthor;			// 发布者
	CString		m_StrDate;				// 日期
	CString		m_StrTime;				// 时间
	CString		m_StrContent;			// 内容
};

// 基本参数
class CBaseInfo
{
public:

	CBaseInfo(){}
	~CBaseInfo(){}

public:
	// 第一条记录
	CString		m_StrTraderName;		// 1	交易商名称
	//float		m_afReserver[24];		// 2-25 保留字段
	CString		m_StrReserver1[6];		// 2-7  保留
	CString		m_StrBalanceTimeInDay;	// 8	每日结算时间
	CString		m_StrReserver2[17];		// 9-25	保留

	// 第二条记录
	float		m_afReserver2[4];		// 1-4  保留字段
	CString		m_StrHistoryIP;			// 5	历史数据IP
	int			m_iHistoryPort;			// 6	历史数据端口
	float		m_afReserver3[9];		// 7-15 保留字段
	int			m_iFlagDayCharge;		// 16   当日结算手续费标志
	CString		m_StrTradeDate;			// 17	交易日期
	float		m_afReserver4[11];		// 18-28保留字段
};

// CAccountInfo 中的资金信息
typedef struct T_FundInfo
{
public:
	double		m_fBalance;				// 1	余额	
	float		m_afReserver[5];		// 2	[备用字段]											
	double		m_fDepositUseage;		// 7	占用保证金额	
	float		m_fReserver8;			// 8	[备用字段]	
	double		m_fBalancePre;			// 9	上日余额	
	double		m_fEarnToday;			// 10	今日平仓盈亏金额	
	double		m_fInToday;				// 11	今日入金额	
	double		m_fOutToday;			// 12	今日出金金额	
	double		m_fFactorageToday;		// 13	今日手续费	
	CString		m_StrCurrencyCode;		// 14	货币代码

}T_FundInfo;

// 帐号信息
class CAccountInfo
{
public:

	CAccountInfo(){}
	~CAccountInfo(){}

public:
	// 第一段
	CString		m_StrUserID;			// 客户帐号

	// 第二段, 客户基本信息
	CString		m_StrPwd;				// 1	密码
	CString		m_StrBrokerCode;		// 2	经纪人代码
	CString		m_StrAccountName;		// 3	账户名称
	CString		m_StrAccountNameEn;		// 4	账户名称(英文)
	CString     m_StrAddress1;			// 5	地址1
	CString     m_StrAddress2;			// 6	地址2
	CString     m_StrPhone1;			// 7	电话1
	CString     m_StrPhone2;			// 8	电话2
	CString     m_StrMobile;			// 9	手机号码
	CString		m_StrFax;				// 10	传真
	int			m_iFlagAllowIn;			// 11	允许下单标志(未用)
	int			m_iFlagShowCharge;		// 12	显示手续费标志(未用)
	int			m_iGroupCode;			// 13	组代码
	int			m_iTypeAccount;			// 14	帐号类型(未用)
	CString		m_StrCurrencyCodeUnUse;	// 15	货币代码(未用)
	int			m_iTypeUser;			// 16	用户类型(未用)
	double		m_fLineOfCredit;		// 17	信用额度
	double		m_fMaxDeposit;			// 18	最大保证金
	int			m_iLevelControl;		// 19	控制级别(未用)
	int			m_iState;				// 20	42正常，43冻结，44只读，45已销户
	float		m_fReserver21;			// 21	备用字段
	long		m_lColor;				// 22	颜色(未用)
	CString		m_StrCity;				// 23	城市
	CString		m_StrArea;				// 24	地区
	CString		m_StrPostCode;			// 25	邮政编码
	CString		m_StrEmail;				// 26	电子邮件
	CString		m_StrPS;				// 27	附注
	CString		m_StrPaperNum;			// 28	证件号码
	CString		m_StrProxyCode;			// 29	代理商代码
	CString		m_StrInvestPwd;			// 30	投资密码
	CString		m_StrPhonePwd;			// 31	电话密码
	CString		m_StrOpenDate;			// 32	开户日期
	CString		m_StrOpenTime;			// 33	开户时间
    int			m_iFlagAllowChgPwd;		// 34	允许修改密码标志
	int			m_iFlagReadOnly;		// 35	只读标志
	int			m_iFlagSendReport;		// 36	发送报告标志
	float		m_fTimesDeposit;		// 37	保证金倍数
	float		m_fRateTax;				// 38	税率
	CString		m_StrLogInTime;			// 39	登录时间
	CString		m_StrLogOutTime;		// 40	登出时间
    float		m_fReserver41;			// 41	[备用字段]
	int			m_iLogInTimes;			// 42	登录次数
	float		m_fReserver43;			// 43	[备用字段]
	
	// 第三段 资金信息
	CArray<T_FundInfo, T_FundInfo&>	m_aFundInfos;

	// 第四段 商品信息
	CArray<CForexMerch,	CForexMerch&> m_aMerchInfos;	
	
	// 第五段
	int			m_iInternalNo;			// 内部编号
};

// 成交信息
class CBargain
{
public:

	CBargain() {};
	~CBargain(){}

public:
	CString		m_StrUserID;		// 1	客户帐号	
	CString		m_StrMerchCode;		// 2	商品代码	
	CString		m_StrDate;			// 3	成交日期	
	CString		m_StrTime;			// 4	成交时间	
	int			m_iBargainNo;		// 5	成交单编号	
	int			m_iColseNo;			// 6	平仓单编号	
	int			m_iInternalNo;		// 7	内部编号	
	CString		m_StrOperatorCode;	// 8	操作员代码	
	CString		m_StrPS;			// 9	备注	
	int			m_iOperateFlag;		// 10	买卖	40:买入,41:卖出
	int			m_iCloseFlag;		// 11	平仓单标志	
	float		m_fVolume;			// 12	数量	
	float		m_fPrice;			// 13	价格	
	float		m_fPriceStopFall;	// 14	止损价格	委托单中的止损价格(显示用)
	float		m_fPriceStopRise;	// 15	止盈价格	委托单中的止盈价格(显示用)
	float		m_fReserver16;		// 16	[备用字段]	
	float		m_fCharge;			// 17	手续费	
	float		m_fInterest;		// 18	利息	
	float		m_fReserver19;		// 19	[备用字段]	
	float		m_fBillCharge;		// 20	仓租	
	float		m_fReserver21;		// 21	[备用字段]	
};

// 委托单信息	(客户端发出委托指令及服务器端返回委托指令接收情况也使用本指令,通过委托单类型来判断进行处理)
class COrderInfo
{
public:

	COrderInfo(){}
	~COrderInfo(){}

public:
	CString		m_StrUserID;				// 1	客户帐号	
	CString		m_StrMerchCode;				// 2	商品代码	
	CString		m_StrOrderDate;				// 3	委托日期	YYYYMMDD
	int			m_iInternalNo;				// 4	内部编号	自定
	CString		m_StrOrderTime;				// 5	委托时间	HHNNSS
	int			m_iOrderNo;					// 6	委托单编号	委托单编号,系统所有委托单编号都是唯一的,委托单成交后会转为成交单编号
	int			m_iCloseNo;					// 7	平仓单编号	如果是平仓单,则本字段填需要平的开仓单的委托单编号
	int			m_iOrderType;				// 8	委托类型:
											//				A:客户端委托指令	60:市价新单,61:限价新单,62:市价平仓单,63:限价平仓单,64:限价买入,65:限价卖出,66:止损买入,67:止捐卖出,68:修改限价平仓单,69:删除限价平仓单,71:客户端主动取消市价单,77:竞价单
											//				B:服务器端返回指令1	70:市场价格已改变(客户委托为市价单指令时,需要把传上去的价格同服务器上的最新价格相比较,如果两都相差超过规定值,则返回本标志,此时第15字段就是服务器上的最新买入价格,需要根据委托单的买卖情况显示出来给客户看,并提示客户是否需要以此价格再次进行委托申报)
											//				C:服务器端返回指令2	64,65,66,67(含义同A),表明委托单已被服务器接受,处于等待成交阶段
											//				D:服务器端返回指令3	69,71:本委托单已被撤单
											//				E:服务器端返回指令4	委托单已被服务器拒绝,拒绝原因编码在29字段(详细描述见错误编码)
	CString		m_StrOperatorCode;			// 9	操作员代码	
	int			m_iValidBillType;			// 10	挂单有效期类型	91：一直有效，92：日内有效（包括止赢止损）,93:日内有效(除止赢止损)，110：挂单本日，止赢止损本周，111：挂单本周，止赢止损本日
	int			m_iBuySell;					// 11	买卖	40:买入,41:卖出
	int			m_iState;					// 12	状态	25:初始状态,26:工作中,27:全部成交,28:部分成交,29:撤单,31:无效,32:提交中,33:增加中,34:改变中,35:删除中,36:无效中
	int			m_iCloseFlag;				// 13	平仓单标志	
	float		m_fVolume;					// 14	数量	
	float		m_fPrice;					// 15	价格	
	float		m_fVolumeSuccess;			// 16	已成交数量	
	float		m_fPriceStopFall;			// 17	止损价格	
	float		m_fPriceStopRise;			// 18	止盈价格	
	float		m_fPointDiff;				// 19	允许点差	
	float		m_fReserver20;				// 20	[备用字段]	填0
	float		m_fReserver21;				// 21	[备用字段]	填0
	float		m_fReserver22;				// 22	[备用字段]	填0
	float		m_fReserver23;				// 23	[备用字段]	填0
	float		m_fReserver24;				// 24	[备用字段]	
	float		m_fReserver25;				// 25	[备用字段]	
	float		m_fReserver26;				// 26	[备用字段]	
	float		m_fReserver27;				// 27	[备用字段]	
	CString		m_StrPS;					// 28	备注	
	//float		m_fReserver29;				// 29	[备用字段]	填0							
	float		m_fPriceInUSD;				// 29	商品用美元表示的价格
	float		m_fReserver30;				// 30	[备用30]
	float		m_fReserver31;				// 31	[备用31]
};

// 已结算平仓单信息(历史成交信息)
class CHistoryOrderInfo
{
public:

	CHistoryOrderInfo(){}
	~CHistoryOrderInfo(){}

public:
	CString		m_StrTradeDate;				// 0	交易日期	
	CString		m_StrUserID;				// 1	客户帐号	
	CString		m_StrMerchCode;				// 2	商品代码	
	float		m_fVolume;					// 3	数量	
	CString		m_StrOpenDate;				// 4	开仓日期	
	CString		m_StrOpenTime;				// 5	开仓时间	
	int			m_iOpenNo;					// 6	开仓单编号	
	int			m_iOpenInternalNo;			// 7	开仓单内部编号	
	CString		m_StrOpenOperatorCode;		// 8	开仓单操作员代码	
	CString		m_StrOpenPS;				// 9	开仓单备注	
	int			m_iOpenBuySell;				// 10	开仓单买卖	
											//		A、交易记录	40:买入,41:卖出
											//		B、出入金记录	142:入金,143:出金,此时21字段对应具体的出入金金额
	float		m_fPriceOpen;				// 11	开仓单价格	
	float		m_fPriceStopRise;			// 12	开仓单止赢价格	
	float		m_fPriceStopFall;			// 13	开仓单止损价格	
	CString		m_StrCloseDate;				// 14	平仓日期	
	CString		m_StrCloseTime;				// 15	平仓时间	
	int			m_iCloseNo;					// 16	平仓单编号	
	int			m_iCloseInternalNo;			// 17	平仓单内部编号	
	CString		m_StrCloseOperatorCode;		// 18	平仓单操作员代码	
	CString		m_StrClosePS;				// 19	平仓单备注	
	float		m_fPriceClose;				// 20	平仓价格	
	float		m_fEarn;					// 21	盈亏	
	float		m_fReserver22;				// 22	[备用字段]	
	float		m_fInterest;				// 23	利息	
	float		m_fCharge;					// 24	手续费	
	float		m_fBillCharge;				// 25	仓租	
};

// 记录信息
class CForexLogInfo
{
public:
	CString		m_StrLog;			// 记录内容
	CString		m_StrTime;			// 时间
};

#endif	// _FOREX_STRUCT_H_