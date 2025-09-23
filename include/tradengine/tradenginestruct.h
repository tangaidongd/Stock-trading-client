#if !defined(_TRADE_ENGINE_STRUCT_H_)
#define _TRADE_ENGINE_STRUCT_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>
using namespace std;

// 查验证码
class CClientQueryCode
{
public:
	void Clear()
	{
		code = L"1";

	}

	CClientQueryCode()
	{
		Clear();
	}

	//
	CString		code; // 此字段无意义，只是用来验证，直接赋1
};

// 查验证码回包
class CClientRespCode
{
public:
	void Clear()
	{
		valid_code_exist = 0;
		valid_code = L"";
		valid_code_len = 0;
		valid_id = L"";
		error_no = 0;
		error_info = L"";
	}

	CClientRespCode()
	{
		Clear();
	}

	//
	int valid_code_exist;	// 网关是否提供验证码
	CString valid_code;		// 验证码
	int valid_code_len;		// 验证码长度
	CString valid_id;		// 验证ID

	int		error_no;		// 错误码 =0:成功，其他失败
	CString	error_info;		// 错误信息
};

// 请求登陆
class CClientReqLogIn
{
public:
	void Clear()
	{
		account = L"";
		password = L"";
		valid_code = L"";
		valid_id = L"";
	}

	CClientReqLogIn()
	{
		Clear();
	}

	//
	CString		account;		// 登录名
	CString		password;		// 密码
	CString		valid_code;		// 验证码
	CString		valid_id;		// 验证ID
};

// 登陆回包
class CClientRespLogIn
{
public:
	void Clear()
	{
		session_no = 0;				
		account = L"";				
		identity= L"";				
		change_password= L"";		
		module_id= L"";				
		user_session_id= L"";		
		user_session_info= L"";		
		thread_id = 0;		
		quote_flag = L"0";
		market_type = L"";
	}

	CClientRespLogIn()
	{
		Clear();
	}
	
	//
	int32		session_no;				// 其他请求时放在 _Head 中 uSessionNo返回给服务器
	CString		account;				// 账号
	CString		identity;				// 用户身份 "0"-客户 "1"-会员(如果是会员，委托下单时可填对方id)
    CString		change_password;		// 是否需要修改密码 "0"-不需要 "1"-需要						 
	CString		module_id;				// 模块ID 							
    CString		user_session_id;		// 在以后每个请求中填写
	CString		user_session_info;		//	session_info 在以后每个请求中填写 仅适用于于多元做市商交易
	int32		thread_id;				// 线程id 注：其他请求时放在 _Head 中 uReserve返回给服务器	
	CString		quote_flag;				// 行情标识“0”-交易提供行情“1”-交易不提供行情 其他默认“0”处理 
	CString		market_type;			// 市场代码 配合quote_flag使用，quote_flag==“1”时需要从行情取交易行情价格
};

// 错误包 
class CClientRespError
{
public:
	void Clear()
	{
		error_no = 0;
		error_info = L"";
	}

	CClientRespError()
	{
		Clear();
	}

	int32		error_no;				// 错误码 =0:成功，其他失败
	CString		error_info;				// 错误信息
};

// 心跳包回包
typedef CClientRespError CClientRespHeart; 

// 登出回包	 
typedef	CClientRespError CClientRespLogOut;

// 请求客户信息
class CClientReqUserInfo
{
public:
	void Clear()
	{
		account = L"";
		user_session_id = L"";
		user_session_info = L"";
		client_type = 0;
	}

	CClientReqUserInfo()
	{
		Clear();
	}

	//
	CString		account;
	CString		user_session_id;
	CString		user_session_info;
	int			client_type;		// 客户端类型 0PC，1Web，默认0
};

// 客户信息回包
class CClientRespUserInfo
{
public:
	void Clear()
	{
		trade_id = L"";		
		first_balance = 0.;	
		last_caution = 0.;	
		balance = 0.;		
		io_fund = 0.;		
		booking_income = 0.;
		today_balance = 0.; 
		today_enable = 0.;	
		today_fee = 0.;		
		today_caution = 0.;	
		yesterday_delay = 0.;
		froze_margin = 0.;	
		froze_fee = 0.;		
		trade_name = L"";	
		impawn = 0.;		
		status = L"";		
		risk = 0.;			
		zt_fund = 0.;		
		mid_code = L"";		
	}

	CClientRespUserInfo()
	{
		Clear();
	}

	//
	CString	trade_id;		// 交易商ID
	double	first_balance;	// 期初余额
	double	last_caution;	// 上日保证金
	double	balance;		// 当前权益
	double  io_fund;		// 当日出入金	
	double  booking_income; // 浮动盈亏
	double  today_balance;  // 今日转让盈余(当日平仓盈亏合计)
	double	today_enable;	// 可用保证金
	double	today_fee;		// 当日手续费
	double	today_caution;	// 占用保证金
	double	yesterday_delay;// 上日延期费
	double  froze_margin;	// 冻结保证金
	double	froze_fee;		// 冻结手续费
	CString trade_name;		// 账户名称
	double	impawn;			// 质押资金
	CString	status;			// "0"-正常 "1"-未激活 "2"-冻结 "3"-终止 "4"-已创建
	double	risk;			// 资金风险率
	double	zt_fund;		// 在途资金
	CString	mid_code;		// mid 在以后其他请求中填写 仅适用于于多元做市商交易
};

// 请求商品信息
class CClientReqMerchInfo
{
public:
	void Clear()
	{
		//
		account = L"";
		user_session_id = L"";
		user_session_info = L"";
		mid_code = L"";	
		stock_code = L"";
	}

	CClientReqMerchInfo()
	{
		Clear();
	}

	//
	CString		account;
	CString		user_session_id;
	CString		user_session_info;
	CString		mid_code;	// 从 查询客户账号信息 应答中获取		
	CString		stock_code;	//	商品代码 空-查全部	
};

// 回包商品信息
typedef struct T_TradeMerchInfo
{
public:
	void Clear()
	{
		stock_code = L"";		
		trade_unit = L"";		
		price_minchange = 0.;	
		stock_name = L"";		
		bail_type = 0;			
		margin_ratio = 0.;		
		charge_con_type = 0;	
		charge_ratio = 0.;		
		charge_tra_type = 0;	
		delay_type = 0;			
		delay_ratio = L"";		
		status = 0;				
		max_entrust_amount = 0;	
		min_entrust_amount = 0;	
		max_hold_amount = 0;	
		step_start = L"";		
		step_end = L"";			
		last_close = 0.;		
		buy_pdd = 0.;			
		sell_pdd = 0.;			
		stop_loss_pdd = 0.;		
		stop_profit_pdd = 0.;	
		open_buy_pdd = 0.;		
		open_sell_pdd = 0.;		
		min_pdd = 0.;			
		max_pdd = 0.;			
		default_pdd = 0.;		
		quote_rate = 0.;	 	
		stock_unit = L"";		
		last_price = 0.;		
		buy_price = 0.;			
		sell_price = 0.;		
		up_price = 0.;			
		down_price = 0.;		
		quote_type = L"";		
		price_mode = L"";		
	}
	
	T_TradeMerchInfo()
	{
		Clear();
	}

	CString	stock_code;			// 商品代码
	CString	trade_unit;			// 交易单位
	double	price_minchange;	// 最小变动价位
	CString	stock_name;			// 商品名称
	int32	bail_type;			// 保证金算法 "1"-按百分比 "2"-按交易数量		
	double	margin_ratio;		// 保证金系数
	int32	charge_con_type;	// 手续费算法 "1"-按百分比 "2"-按交易数量		
	double	charge_ratio;		// 手续费系数
	int32	charge_tra_type;	// 手续费收取方式 "1"-单边建仓 "2"-单边平仓  "3"-双边建平 "4"-隔夜建平
	int32	delay_type;			// 延期费算法 "1"-按百分比 "2"-按交易数量	
	CString	delay_ratio;		// 延期费率（数组）  每个值中间用'|'隔开，并以'|'结束		
	int32	status;				// 商品状态 "0"-编辑中 "1"-上市 "2"-退市		
	int32	max_entrust_amount;	// 单笔最大可委托数量
	int32	min_entrust_amount;	// 单笔最小可委托数量
	int32	max_hold_amount;	// 最大可持仓量
	CString	step_start;			// 阶梯值起始值（数组）  每个值中间用'|'隔开，并以'|'结束		
	CString	step_end;			// 阶梯值终止值（数组）  每个值中间用'|'隔开，并以'|'结束		
	double	last_close;			// 昨结算
	double	buy_pdd;			// 买价点差
	double	sell_pdd;			// 卖价点差
	double	stop_loss_pdd;		// 止损下单点差
	double	stop_profit_pdd;	// 止盈下单点差
	double	open_buy_pdd;		// 限价建仓买点差
	double	open_sell_pdd;		// 限价建仓卖点差
	double	min_pdd;			// 用户报价点差最小值
	double	max_pdd;			// 用户报价点差最大值
	double	default_pdd;		// 用户报价点差默认值
	double	quote_rate;	 		// 报价汇率
	CString	stock_unit;			// 商品单位
	double	last_price;			// 最新价
	double	buy_price;			// 买价
	double	sell_price;			// 卖价
	double	up_price;			// 最高价
	double	down_price;			// 最低价
	CString	quote_type;			// 商品报价类型 "0"-直接商品 "1"-间接商品（虚盘）"2"-间接商品(实盘)
	CString price_mode;			// 价格计算方法 "0"-取买卖价加买卖价点差成买卖价 "1"-取最新价加买卖价点差成买卖价

}T_TradeMerchInfo;

class CClientRespMerchInfo
{
public:
	CClientRespMerchInfo()
	{
		m_aMerchInfos.clear();
	}

	vector<T_TradeMerchInfo>	m_aMerchInfos;
};

// 请求委托买卖
class CClientReqEntrust
{
public:
	void Clear()
	{
		account = L"";			
		stock_code = L"";		
		entrust_price = L"";			
		entrust_amount = 0;		
		entrust_bs = L"";				
		entrust_prop = L"";				
		eo_flag = L"";			
		entrust_type = L"";		
		
		stop_loss = 0.;			
		stop_profit = 0.;		
		dot_diff = 0.;			
		close_price = 0.;		
		hold_id = L"";			
		other_id = L"";				
		user_session_id = L"";	
		user_session_info = L"";
		mid_code = L"";			
		back_amount = 0;		
	}

	CClientReqEntrust()
	{
		Clear();
	}

	//
	CString	account;			// 账号
	CString	stock_code;			// 商品代码
	CString	entrust_price;		// 商品价格 市价委托时可不填
		
	int32	entrust_amount;		// 委托数量
	CString entrust_bs;			// 买卖类别 "B"-买 "S"-卖
		
		
	CString	entrust_prop;		// 委托属性
										/*
										0	买卖 1	配股 2	转托 3	申购 4	回购 5	配售 6	指定 7	转股 8	回售 9	股息 A	深圳配售放弃 B	无股份冻结质押 C	股份质押冻结 D	无股份解押解冻 E	股份解押解冻
										F	预受要约
										G	解除要约
										H	开放式基金申购赎回
										I	权证行权
										J	存入
										K	取出
										L	开户
										M	销户
										N	冲正
										O	查余额
										P	改密
										Q	取明细
										*/
	CString eo_flag;			// 开平仓标志 "0"-开仓 "1"-平仓 "2"-指定仓单号平仓
	CString	entrust_type;		// 委托类型 "0"-市价委托 "1"-限价委托
						
	double	stop_loss;			// 止损价(限价委托时用)
	double	stop_profit;		// 止盈价(限价委托时用)
	double	dot_diff;			// 点差(市价委托时用)
	double	close_price;		// 平仓价格(指定价格平仓时用)
	CString	hold_id;			// 持仓单号(指定仓单号平仓时用)
	CString	other_id;			// 成交对方ＩＤ(通过506查) 仅适用于金网安泰做市商交易		
	CString	user_session_id;	// session_id 从登录应答中获取		
	CString	user_session_info;	// session_info 从登录应答中获取		
	CString	mid_code;			// mid	从 查询客户账号信息 应答中获取		
	int32	back_amount;		// 反手建仓数量 仅适用于多元做市商交易（市价平仓时选用）
};

// 委托买卖回包
typedef CClientRespError CClientRespEntrust;

// 撤销委托单
class CClientReqCancelEntrust
{
public:
	CClientReqCancelEntrust()
	{
		account = L"";			
		entrust_no = L"";		
		user_session_id = L"";	
		user_session_info = L"";
		mid_code = L"";
	}

	CString account;			// 账号
	CString	entrust_no;			// 委托单号
	CString	user_session_id;	// session_id 从登录应答中获取
	CString	user_session_info;	// session_info 从登录应答中获取
	CString	mid_code;			// mid 从 查询客户账号信息 应答中获取
};

// 撤销委托买卖回包
typedef CClientRespError CClientRespCancelEntrust;

// 查委托
class CClientReqQueryEntrust
{
public:
	void Clear()
	{
		account = L"";			
		query_direction = L"";	
		start_num = 0;	
		request_num = 0;
		user_session_id = L"";	
		user_session_info = L"";
		mid_code = L"";	
	}
	
	CClientReqQueryEntrust()
	{
		Clear();
	}

	//
	CString account;			// 账号
	CString	query_direction;	// 查询方向 "0"-倒序 "1"-顺序
	int32	start_num;			// 起始位置
	int32	request_num;		// 请求行数
	CString	user_session_id;	
	CString	user_session_info;
	CString	mid_code;	
};

// 查委托回包
typedef struct T_RespQueryEntrust
{
public:
	void Clear()
	{
		stock_code = L"";
		entrust_no = L"";
		entrust_bs = L"";
		eo_flag = L"";	
		entrust_status = L"";
		entrust_price = 0.;	
		entrust_amount = 0;	
		entrust_deal = 0;	
		entrust_less = 0;	
		entrust_time = 0;	
		stop_loss = 0.;		
		stop_profit = 0.;	
		froze_margin = 0.;	
		froze_fee = 0.;		
		hold_id = L"";		
		agent_id = L"";		
		total_record = 0;
	}

	T_RespQueryEntrust()
	{
		Clear();
	}

	CString stock_code;		// 证券代码
	CString	entrust_no;		// 委托单号
	CString	entrust_bs;		// 买卖类别 "B"-买 "S"-卖
	CString	eo_flag;		// 开平仓标志 "0"-开仓 "1"-平仓 "2"-指定仓单号平仓
	CString	entrust_status;	//	委托状态
							/*	"0"-未报
								"1"-已报
								"2"-未知
								"3"-部成
								"4"-部撤
								"5"-已撤
								"6"-部成部撤
								"7"-已成
								"8"-已废*/
	double	entrust_price;	// 委托价格
	int32	entrust_amount;	// 委托数量
	int32	entrust_deal;	// 成交数量
	int32	entrust_less;	// 未成数量
	int32	entrust_time;	// 委托时间
	double	stop_loss;		// 止损价
	double	stop_profit;	// 止盈价
	double	froze_margin;	// 冻结保证金
	double	froze_fee;		// 冻结手续费
	CString	hold_id;		// 持仓单号
	CString	agent_id;		// 代为委托员代码
	long	total_record;	// 总记录数

}T_RespQueryEntrust;

// 查询可撤单504
typedef CClientReqQueryEntrust CClientReqQueryCancelEntrust;

// 查询可撤单504回包
typedef T_RespQueryEntrust T_RespQueryCancelEntrust;

class CClientRespQueryEntrust
{
public:
	CClientRespQueryEntrust() 
	{
		m_aQureyEntrust.clear();
	}

	//
	vector<T_RespQueryEntrust> m_aQureyEntrust;
};

typedef CClientRespQueryEntrust CClientRespQueryCancelEntrust;

// 查成交
class CClientReqQueryDeal
{
public:
	CClientReqQueryDeal()
	{
		account = L"";			
		user_session_id = L"";	
		user_session_info = L"";	
		mid_code = L"";			
	}

	//
	CString account;			// 账号
	CString	user_session_id;	// session_id 从登录应答中获取
	CString	user_session_info;	// session_info 从登录应答中获取
	CString	mid_code;			// mid 从 查询客户账号信息 应答中获取
};

typedef struct T_RespQueryDeal
{
public:
	void Clear()
	{
		stock_code = L"";		
		entrust_no = L"";		
		deal_no = L"";			
		business_time = L"";	
		entrust_bs = L"";		
		eo_flag = L"";			
		business_price = 0.;	
		business_amount = 0;	
		close_profitloss = 0.;	
		charge = 0.;			
		trade_type = L"";								
		hold_id = L"";			
		entrust_price = 0.;		
		close_price = 0.;		
	}
	
	T_RespQueryDeal()
	{
		Clear();
	}

	//
	CString stock_code;			// 证券代码
	CString	entrust_no;			// 委托单号
	CString	deal_no;			// 成交单号
	CString	business_time;		// 成交时间
	CString	entrust_bs;			// 买卖类别 "B"-买 "S"-卖
	CString	eo_flag;			// 开平仓标志 "0"-开仓 "1"-平仓 "2"-指定仓单号平仓
	double	business_price;		// 成交价格
	int32	business_amount;	// 成交数量
	double	close_profitloss;	// 平仓盈亏
	double	charge;				// 手续费
	CString	trade_type;			// 成交类型
								/*
								"1"-市价成交
								"2"-市价成交
								"3"-市价成交
								"4"-自动强平
								"5"-手动强平
								"6"-指价成交
								"7"-指价成交
								*/
	CString	hold_id;			// 持仓单号
	double	entrust_price;		// 委托价格
	double	close_price;		// 平仓价格
}T_RespQueryDeal;

// 查成交回包
class CClientRespQueryDeal
{
public:
	CClientRespQueryDeal()
	{
		m_aQueryDeal.clear();
	}

	//
	vector<T_RespQueryDeal>	m_aQueryDeal;
};

// 查持仓
class CClientReqQueryHold
{
public:
	CClientReqQueryHold()
	{
		account = L"";			
		query_direction = L"";			
		start_num = 0;			
		request_num = 0;		
		user_session_id = L"";	
		user_session_info = L"";
		mid_code = L"";			
	}

	//
	CString		account;			// 账号	
	CString		query_direction;	// 查询方向 	"0"-倒序 "1"-顺序		
	int32		start_num;			// 起始位置
	int32		request_num;		// 请求行数
	CString		user_session_id;	// session_id	
	CString		user_session_info;	// session_info	
	CString		mid_code;			// mid
};

typedef struct T_RespQueryHold
{
public:
	void Clear()
	{
		stock_code = L"";	
		entrust_bs = L"";	
		hold_amount = 0;	
		hold_enable = 0;	
		avg = 0.;		
		open_price = 0.;
		hold_price = 0.;
		floating_profit = 0.;
		margin = 0.;		
		hold_avg = 0.;		
		froze_amount = 0;	
		hold_id = L"";		
		stop_loss = 0.;			
		stop_profit = 0.;		
		entrust_time = L"";		
		stop_profit_no = L"";		
		stop_loss_no = L"";	
	}

	T_RespQueryHold()
	{
		Clear();
	}

	//
	CString stock_code;		// 证券代码
	CString	entrust_bs;		// 买卖类别 "B"-买 "S"-卖
	int32	hold_amount;	// 持仓数量
	int32	hold_enable;	// 持仓可用
	double	avg;			// 建仓均价
	double	open_price;		// 开仓价...
	double	hold_price;		// 持仓价...
	double	floating_profit;// 浮动盈亏
	double	margin;			// 保证金
	double	hold_avg;		// 持仓均价
	int32	froze_amount;	// 冻结数量
	CString	hold_id;		// 持仓单号 仅适用于多元做市商
	double	stop_loss;		// 止损价 仅适用于多元做市商		
	double	stop_profit;	// 止盈价 仅适用于多元做市商		
	CString	entrust_time;	// 建仓时间 仅适用于多元做市商		
	CString	stop_profit_no;	// 止盈好 仅适用于多元做市商		
	CString	stop_loss_no;	// 止损号 仅适用于多元做市商

}T_RespQueryHold;

class CClientRespQueryHold
{
public:
	CClientRespQueryHold()
	{
		m_aQueryHold.clear();
	}

	//
	vector<T_RespQueryHold>	m_aQueryHold;
};

// 查持仓汇总
class CClientReqQueryHoldTotal
{
public:
	CClientReqQueryHoldTotal()
	{
		account = L"";			
		query_direction = L"";			
		start_num = 0;			
		request_num = 0;		
		user_session_id = L"";	
		user_session_info = L"";
		mid_code = L"";		
		client_type = 0;
	}

	//
	CString		account;			// 账号	
	CString		query_direction;	// 查询方向 	"0"-倒序 "1"-顺序		
	int32		start_num;			// 起始位置
	int32		request_num;		// 请求行数
	CString		user_session_id;	// session_id	
	CString		user_session_info;	// session_info	
	CString		mid_code;			// mid
	int			client_type;		// 客户端类型
};

typedef struct T_RespQueryHoldTotal
{
public:
	void Clear()
	{
		stock_code = L"";	
		entrust_bs = L"";	
		hold_amount = 0;	
		hold_enable = 0;	
		avg = 0.;			
		floating_profit = 0.;
		margin = 0.;		
		hold_avg = 0.;		
		froze_amount = 0;	
		hold_id = L"";		
		stop_loss = 0.;			
		stop_profit = 0.;		
		entrust_time = L"";		
		stop_profit_no = L"";		
		stop_loss_no = L"";	
		total_record = 0;
	}

	T_RespQueryHoldTotal()
	{
		Clear();
	}

	//
	CString stock_code;		// 证券代码
	CString	entrust_bs;		// 买卖类别 "B"-买 "S"-卖
	int32	hold_amount;	// 持仓数量
	int32	hold_enable;	// 持仓可用
	double	avg;			// 建仓均价
	double	floating_profit;// 浮动盈亏
	double	margin;			// 保证金
	double	hold_avg;		// 持仓均价
	int32	froze_amount;	// 冻结数量
	CString	hold_id;		// 持仓单号 仅适用于多元做市商
	double	stop_loss;		// 止损价 仅适用于多元做市商		
	double	stop_profit;	// 止盈价 仅适用于多元做市商		
	CString	entrust_time;	// 建仓时间 仅适用于多元做市商		
	CString	stop_profit_no;	// 止盈号 仅适用于多元做市商		
	CString	stop_loss_no;	// 止损号 仅适用于多元做市商
	long	total_record;	// 总记录数

}T_RespQueryHoldTotal;

class CClientRespQueryHoldTotal
{
public:
	CClientRespQueryHoldTotal()
	{
		m_aQueryHoldTotal.clear();
	}

	//
	vector<T_RespQueryHoldTotal>	m_aQueryHoldTotal;
};

// 请求行情
class CClientReqQuote
{
public:
	CClientReqQuote()
	{
		account = L"";			
		stock_code = L"";		
		user_session_id = L"";	
		user_session_info = L"";
		mid_code = L"";			
	}

	//
	CString		account;			// 账号	
	CString		stock_code;			// 商品代码	为空的时候 - 查全部
	CString		user_session_id;	// session_id	
	CString		user_session_info;	// session_info	
	CString		mid_code;			// mid
};

typedef struct T_RespQuote
{
public:
	void Clear()
	{
		stock_code = L""; 	
		up_price = 0.;		
		down_price = 0.;	
		last_price = 0.;	
		buy_price = 0.;
		sell_price = 0.;				
		quote_time = L"";
	}

	T_RespQuote()
	{
		Clear();
	}

	//
	CString		stock_code; 	// 商品代码
	double		up_price;		// 最高价
	double		down_price;		// 最低价
	double		last_price;		// 最新价
	
	double		buy_price;		// 买价
								/*
								注意，针对多元做市商交易，此处的买价并非是真正的盘口买价，计算公式如下：
								查商品信息（204）里面的 price_mode 字段：
								price_mode 为 "0"时：
								买价 = buy_price + price_minchange* buy_pdd
								
								price_mode 为 "1"时：
								买价 = last_price + price_minchange* buy_pdd
								  
								price_minchange buy_pdd两个字段请查看商品信息应答，此公式同样适用于 查商品信息中的买价）									
								*/
	
	double		sell_price;		// 卖价
								/*
								注意，针对多元做市商交易，此处的卖价并非是真正的盘口卖价，计算公式如下：
								查商品信息（204）里面的 price_mode 字段：
								price_mode 为 "0"时：
								买价 = sell_price + price_minchange* sell_pdd
								
								price_mode 为 "1"时：
								买价 = last_price + price_minchange* sell_pdd
								  
								（price_minchange sell_pdd两个字段请查看商品信息应答，此公式同样适用于 查商品信息中的卖价）
								*/

	CString		quote_time;		// 报价时间

}T_RespQuote;

// 应答行情
class CClientRespQuote
{
public:
	CClientRespQuote()
	{
		m_aQuote.clear();
	}

	//
	vector<T_RespQuote>	m_aQuote;
};

// 设置止盈止损
class CClientReqSetStopLP
{
public:
	void Clear()
	{
		account = L"";		
		hold_id = L"";		
		stop_loss = 0.;		
		stop_profit = 0.;	
		entrust_bs = L"";	
		stock_code = L"";	
		entrust_price = 0.;	
		trade_flag = L"";	
		user_session_id = L"";		
		user_session_info = L"";	
		mid_code = L"";				
	}

	CClientReqSetStopLP()
	{
		Clear();
	}

	//
	CString	account;		// 账号
	CString	hold_id;		// 持仓单号
	double	stop_loss;		// 止损价
	double	stop_profit;	// 止盈价
	CString	entrust_bs;		// 买卖类别 "B"-买 "S"-卖
	CString	stock_code;		// 商品代码
	double	entrust_price;	// 委托价格
	CString	trade_flag;		// 下单标识 "1"-修改限价单  "2"-设置止盈止损
	CString	user_session_id;		// session_id	
	CString	user_session_info;		// session_info	
	CString	mid_code;				// mid
};

typedef CClientRespError CClientRespSetStopLP;


// 取消设置止盈止损
class CClientReqCancelStopLP
{
public:
	CClientReqCancelStopLP()
	{
		account = L"";				
		hold_id = L"";				
		cancel_profitloss_type = L"";
		stock_code = L"";			
		entrust_no = L"";			
		user_session_id = L"";		
		user_session_info = L"";	
		mid_code = L"";				
	}
	
	//
	CString	account;				// 账号
	CString	hold_id;				// 持仓单号
	CString	cancel_profitloss_type;	// 撤销止损止盈价格类别 "1"-撤销止损价 "2"-撤销止盈价 "3"-撤销止损止盈价
	CString	stock_code;				// 商品代码
	CString	entrust_no;				// 止盈/止损号 撤止盈时填止盈号，撤止损时填止损号，止盈止损号从查持仓明细（505）应答取
	CString	user_session_id;		// session_id	
	CString	user_session_info;		// session_info	
	CString	mid_code;				// mid																	
};

typedef CClientRespError CClientRespCancelStopLP;

// 修改密码
class CClientReqModifyPwd
{
public:
	CClientReqModifyPwd()
	{
		account = L"";			
		password_type = L"0";		
		password = L"";	
		new_password = L"";
		user_session_id = L"";
		user_session_info = L"";
		mid_code = L"";
	}
	
	CString account;			// 账号
	CString	password_type;		// 密码类型（"0"-交易密码，"1"-电话密码，"2"-资金密码）
	CString	password;			// 原密码
	CString	new_password;		// 新密码
	CString	user_session_id;	// session_id从登录应答中获取
	CString	user_session_info;	// session_info从登录应答中获取
	CString	mid_code;			// mid从 查询客户账号信息 应答中获取
};

// 修改密码回包
typedef CClientRespError CClientRespModifyPwd;

// 查对方交易员ID （506）
class CClientQueryTraderID
{
public:
	CClientQueryTraderID()
	{
		account = L"";			
		user_session_id = L"";
		user_session_info = L"";
		mid_code = L"";
	}

	CString account;			// 账号
	CString	user_session_id;	// session_id从登录应答中获取
	CString	user_session_info;	// session_info从登录应答中获取
	CString	mid_code;			// mid从 查询客户账号信息 应答中获取
};

// 查对方交易员ID回包
typedef struct T_RespTraderID
{
public:
	void Clear()
	{
		other_id = L"";
		other_name = L"";
	}

	T_RespTraderID()
	{
		Clear();
	}

	CString other_id;	// 对方交易员ID
	CString other_name;	// 对方交易员名称
}T_RespTraderID;

class CClientRespTraderID
{
public:
	CClientRespTraderID()
	{
		m_aQueryTraderID.clear();
	}

	//
	vector<T_RespTraderID>	m_aQueryTraderID;
};

#endif // _TRADE_ENGINE_STRUCT_H_