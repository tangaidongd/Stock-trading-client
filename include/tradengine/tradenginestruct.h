#if !defined(_TRADE_ENGINE_STRUCT_H_)
#define _TRADE_ENGINE_STRUCT_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>
using namespace std;

// ����֤��
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
	CString		code; // ���ֶ������壬ֻ��������֤��ֱ�Ӹ�1
};

// ����֤��ذ�
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
	int valid_code_exist;	// �����Ƿ��ṩ��֤��
	CString valid_code;		// ��֤��
	int valid_code_len;		// ��֤�볤��
	CString valid_id;		// ��֤ID

	int		error_no;		// ������ =0:�ɹ�������ʧ��
	CString	error_info;		// ������Ϣ
};

// �����½
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
	CString		account;		// ��¼��
	CString		password;		// ����
	CString		valid_code;		// ��֤��
	CString		valid_id;		// ��֤ID
};

// ��½�ذ�
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
	int32		session_no;				// ��������ʱ���� _Head �� uSessionNo���ظ�������
	CString		account;				// �˺�
	CString		identity;				// �û���� "0"-�ͻ� "1"-��Ա(����ǻ�Ա��ί���µ�ʱ����Է�id)
    CString		change_password;		// �Ƿ���Ҫ�޸����� "0"-����Ҫ "1"-��Ҫ						 
	CString		module_id;				// ģ��ID 							
    CString		user_session_id;		// ���Ժ�ÿ����������д
	CString		user_session_info;		//	session_info ���Ժ�ÿ����������д ���������ڶ�Ԫ�����̽���
	int32		thread_id;				// �߳�id ע����������ʱ���� _Head �� uReserve���ظ�������	
	CString		quote_flag;				// �����ʶ��0��-�����ṩ���顰1��-���ײ��ṩ���� ����Ĭ�ϡ�0������ 
	CString		market_type;			// �г����� ���quote_flagʹ�ã�quote_flag==��1��ʱ��Ҫ������ȡ��������۸�
};

// ����� 
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

	int32		error_no;				// ������ =0:�ɹ�������ʧ��
	CString		error_info;				// ������Ϣ
};

// �������ذ�
typedef CClientRespError CClientRespHeart; 

// �ǳ��ذ�	 
typedef	CClientRespError CClientRespLogOut;

// ����ͻ���Ϣ
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
	int			client_type;		// �ͻ������� 0PC��1Web��Ĭ��0
};

// �ͻ���Ϣ�ذ�
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
	CString	trade_id;		// ������ID
	double	first_balance;	// �ڳ����
	double	last_caution;	// ���ձ�֤��
	double	balance;		// ��ǰȨ��
	double  io_fund;		// ���ճ����	
	double  booking_income; // ����ӯ��
	double  today_balance;  // ����ת��ӯ��(����ƽ��ӯ���ϼ�)
	double	today_enable;	// ���ñ�֤��
	double	today_fee;		// ����������
	double	today_caution;	// ռ�ñ�֤��
	double	yesterday_delay;// �������ڷ�
	double  froze_margin;	// ���ᱣ֤��
	double	froze_fee;		// ����������
	CString trade_name;		// �˻�����
	double	impawn;			// ��Ѻ�ʽ�
	CString	status;			// "0"-���� "1"-δ���� "2"-���� "3"-��ֹ "4"-�Ѵ���
	double	risk;			// �ʽ������
	double	zt_fund;		// ��;�ʽ�
	CString	mid_code;		// mid ���Ժ�������������д ���������ڶ�Ԫ�����̽���
};

// ������Ʒ��Ϣ
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
	CString		mid_code;	// �� ��ѯ�ͻ��˺���Ϣ Ӧ���л�ȡ		
	CString		stock_code;	//	��Ʒ���� ��-��ȫ��	
};

// �ذ���Ʒ��Ϣ
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

	CString	stock_code;			// ��Ʒ����
	CString	trade_unit;			// ���׵�λ
	double	price_minchange;	// ��С�䶯��λ
	CString	stock_name;			// ��Ʒ����
	int32	bail_type;			// ��֤���㷨 "1"-���ٷֱ� "2"-����������		
	double	margin_ratio;		// ��֤��ϵ��
	int32	charge_con_type;	// �������㷨 "1"-���ٷֱ� "2"-����������		
	double	charge_ratio;		// ������ϵ��
	int32	charge_tra_type;	// ��������ȡ��ʽ "1"-���߽��� "2"-����ƽ��  "3"-˫�߽�ƽ "4"-��ҹ��ƽ
	int32	delay_type;			// ���ڷ��㷨 "1"-���ٷֱ� "2"-����������	
	CString	delay_ratio;		// ���ڷ��ʣ����飩  ÿ��ֵ�м���'|'����������'|'����		
	int32	status;				// ��Ʒ״̬ "0"-�༭�� "1"-���� "2"-����		
	int32	max_entrust_amount;	// ��������ί������
	int32	min_entrust_amount;	// ������С��ί������
	int32	max_hold_amount;	// ���ɳֲ���
	CString	step_start;			// ����ֵ��ʼֵ�����飩  ÿ��ֵ�м���'|'����������'|'����		
	CString	step_end;			// ����ֵ��ֵֹ�����飩  ÿ��ֵ�м���'|'����������'|'����		
	double	last_close;			// �����
	double	buy_pdd;			// ��۵��
	double	sell_pdd;			// ���۵��
	double	stop_loss_pdd;		// ֹ���µ����
	double	stop_profit_pdd;	// ֹӯ�µ����
	double	open_buy_pdd;		// �޼۽�������
	double	open_sell_pdd;		// �޼۽��������
	double	min_pdd;			// �û����۵����Сֵ
	double	max_pdd;			// �û����۵�����ֵ
	double	default_pdd;		// �û����۵��Ĭ��ֵ
	double	quote_rate;	 		// ���ۻ���
	CString	stock_unit;			// ��Ʒ��λ
	double	last_price;			// ���¼�
	double	buy_price;			// ���
	double	sell_price;			// ����
	double	up_price;			// ��߼�
	double	down_price;			// ��ͼ�
	CString	quote_type;			// ��Ʒ�������� "0"-ֱ����Ʒ "1"-�����Ʒ�����̣�"2"-�����Ʒ(ʵ��)
	CString price_mode;			// �۸���㷽�� "0"-ȡ�����ۼ������۵��������� "1"-ȡ���¼ۼ������۵���������

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

// ����ί������
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
	CString	account;			// �˺�
	CString	stock_code;			// ��Ʒ����
	CString	entrust_price;		// ��Ʒ�۸� �м�ί��ʱ�ɲ���
		
	int32	entrust_amount;		// ί������
	CString entrust_bs;			// ������� "B"-�� "S"-��
		
		
	CString	entrust_prop;		// ί������
										/*
										0	���� 1	��� 2	ת�� 3	�깺 4	�ع� 5	���� 6	ָ�� 7	ת�� 8	���� 9	��Ϣ A	�������۷��� B	�޹ɷݶ�����Ѻ C	�ɷ���Ѻ���� D	�޹ɷݽ�Ѻ�ⶳ E	�ɷݽ�Ѻ�ⶳ
										F	Ԥ��ҪԼ
										G	���ҪԼ
										H	����ʽ�����깺���
										I	Ȩ֤��Ȩ
										J	����
										K	ȡ��
										L	����
										M	����
										N	����
										O	�����
										P	����
										Q	ȡ��ϸ
										*/
	CString eo_flag;			// ��ƽ�ֱ�־ "0"-���� "1"-ƽ�� "2"-ָ���ֵ���ƽ��
	CString	entrust_type;		// ί������ "0"-�м�ί�� "1"-�޼�ί��
						
	double	stop_loss;			// ֹ���(�޼�ί��ʱ��)
	double	stop_profit;		// ֹӯ��(�޼�ί��ʱ��)
	double	dot_diff;			// ���(�м�ί��ʱ��)
	double	close_price;		// ƽ�ּ۸�(ָ���۸�ƽ��ʱ��)
	CString	hold_id;			// �ֲֵ���(ָ���ֵ���ƽ��ʱ��)
	CString	other_id;			// �ɽ��Է��ɣ�(ͨ��506��) �������ڽ�����̩�����̽���		
	CString	user_session_id;	// session_id �ӵ�¼Ӧ���л�ȡ		
	CString	user_session_info;	// session_info �ӵ�¼Ӧ���л�ȡ		
	CString	mid_code;			// mid	�� ��ѯ�ͻ��˺���Ϣ Ӧ���л�ȡ		
	int32	back_amount;		// ���ֽ������� �������ڶ�Ԫ�����̽��ף��м�ƽ��ʱѡ�ã�
};

// ί�������ذ�
typedef CClientRespError CClientRespEntrust;

// ����ί�е�
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

	CString account;			// �˺�
	CString	entrust_no;			// ί�е���
	CString	user_session_id;	// session_id �ӵ�¼Ӧ���л�ȡ
	CString	user_session_info;	// session_info �ӵ�¼Ӧ���л�ȡ
	CString	mid_code;			// mid �� ��ѯ�ͻ��˺���Ϣ Ӧ���л�ȡ
};

// ����ί�������ذ�
typedef CClientRespError CClientRespCancelEntrust;

// ��ί��
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
	CString account;			// �˺�
	CString	query_direction;	// ��ѯ���� "0"-���� "1"-˳��
	int32	start_num;			// ��ʼλ��
	int32	request_num;		// ��������
	CString	user_session_id;	
	CString	user_session_info;
	CString	mid_code;	
};

// ��ί�лذ�
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

	CString stock_code;		// ֤ȯ����
	CString	entrust_no;		// ί�е���
	CString	entrust_bs;		// ������� "B"-�� "S"-��
	CString	eo_flag;		// ��ƽ�ֱ�־ "0"-���� "1"-ƽ�� "2"-ָ���ֵ���ƽ��
	CString	entrust_status;	//	ί��״̬
							/*	"0"-δ��
								"1"-�ѱ�
								"2"-δ֪
								"3"-����
								"4"-����
								"5"-�ѳ�
								"6"-���ɲ���
								"7"-�ѳ�
								"8"-�ѷ�*/
	double	entrust_price;	// ί�м۸�
	int32	entrust_amount;	// ί������
	int32	entrust_deal;	// �ɽ�����
	int32	entrust_less;	// δ������
	int32	entrust_time;	// ί��ʱ��
	double	stop_loss;		// ֹ���
	double	stop_profit;	// ֹӯ��
	double	froze_margin;	// ���ᱣ֤��
	double	froze_fee;		// ����������
	CString	hold_id;		// �ֲֵ���
	CString	agent_id;		// ��Ϊί��Ա����
	long	total_record;	// �ܼ�¼��

}T_RespQueryEntrust;

// ��ѯ�ɳ���504
typedef CClientReqQueryEntrust CClientReqQueryCancelEntrust;

// ��ѯ�ɳ���504�ذ�
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

// ��ɽ�
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
	CString account;			// �˺�
	CString	user_session_id;	// session_id �ӵ�¼Ӧ���л�ȡ
	CString	user_session_info;	// session_info �ӵ�¼Ӧ���л�ȡ
	CString	mid_code;			// mid �� ��ѯ�ͻ��˺���Ϣ Ӧ���л�ȡ
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
	CString stock_code;			// ֤ȯ����
	CString	entrust_no;			// ί�е���
	CString	deal_no;			// �ɽ�����
	CString	business_time;		// �ɽ�ʱ��
	CString	entrust_bs;			// ������� "B"-�� "S"-��
	CString	eo_flag;			// ��ƽ�ֱ�־ "0"-���� "1"-ƽ�� "2"-ָ���ֵ���ƽ��
	double	business_price;		// �ɽ��۸�
	int32	business_amount;	// �ɽ�����
	double	close_profitloss;	// ƽ��ӯ��
	double	charge;				// ������
	CString	trade_type;			// �ɽ�����
								/*
								"1"-�м۳ɽ�
								"2"-�м۳ɽ�
								"3"-�м۳ɽ�
								"4"-�Զ�ǿƽ
								"5"-�ֶ�ǿƽ
								"6"-ָ�۳ɽ�
								"7"-ָ�۳ɽ�
								*/
	CString	hold_id;			// �ֲֵ���
	double	entrust_price;		// ί�м۸�
	double	close_price;		// ƽ�ּ۸�
}T_RespQueryDeal;

// ��ɽ��ذ�
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

// ��ֲ�
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
	CString		account;			// �˺�	
	CString		query_direction;	// ��ѯ���� 	"0"-���� "1"-˳��		
	int32		start_num;			// ��ʼλ��
	int32		request_num;		// ��������
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
	CString stock_code;		// ֤ȯ����
	CString	entrust_bs;		// ������� "B"-�� "S"-��
	int32	hold_amount;	// �ֲ�����
	int32	hold_enable;	// �ֲֿ���
	double	avg;			// ���־���
	double	open_price;		// ���ּ�...
	double	hold_price;		// �ֲּ�...
	double	floating_profit;// ����ӯ��
	double	margin;			// ��֤��
	double	hold_avg;		// �ֲ־���
	int32	froze_amount;	// ��������
	CString	hold_id;		// �ֲֵ��� �������ڶ�Ԫ������
	double	stop_loss;		// ֹ��� �������ڶ�Ԫ������		
	double	stop_profit;	// ֹӯ�� �������ڶ�Ԫ������		
	CString	entrust_time;	// ����ʱ�� �������ڶ�Ԫ������		
	CString	stop_profit_no;	// ֹӯ�� �������ڶ�Ԫ������		
	CString	stop_loss_no;	// ֹ��� �������ڶ�Ԫ������

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

// ��ֲֻ���
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
	CString		account;			// �˺�	
	CString		query_direction;	// ��ѯ���� 	"0"-���� "1"-˳��		
	int32		start_num;			// ��ʼλ��
	int32		request_num;		// ��������
	CString		user_session_id;	// session_id	
	CString		user_session_info;	// session_info	
	CString		mid_code;			// mid
	int			client_type;		// �ͻ�������
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
	CString stock_code;		// ֤ȯ����
	CString	entrust_bs;		// ������� "B"-�� "S"-��
	int32	hold_amount;	// �ֲ�����
	int32	hold_enable;	// �ֲֿ���
	double	avg;			// ���־���
	double	floating_profit;// ����ӯ��
	double	margin;			// ��֤��
	double	hold_avg;		// �ֲ־���
	int32	froze_amount;	// ��������
	CString	hold_id;		// �ֲֵ��� �������ڶ�Ԫ������
	double	stop_loss;		// ֹ��� �������ڶ�Ԫ������		
	double	stop_profit;	// ֹӯ�� �������ڶ�Ԫ������		
	CString	entrust_time;	// ����ʱ�� �������ڶ�Ԫ������		
	CString	stop_profit_no;	// ֹӯ�� �������ڶ�Ԫ������		
	CString	stop_loss_no;	// ֹ��� �������ڶ�Ԫ������
	long	total_record;	// �ܼ�¼��

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

// ��������
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
	CString		account;			// �˺�	
	CString		stock_code;			// ��Ʒ����	Ϊ�յ�ʱ�� - ��ȫ��
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
	CString		stock_code; 	// ��Ʒ����
	double		up_price;		// ��߼�
	double		down_price;		// ��ͼ�
	double		last_price;		// ���¼�
	
	double		buy_price;		// ���
								/*
								ע�⣬��Զ�Ԫ�����̽��ף��˴�����۲������������̿���ۣ����㹫ʽ���£�
								����Ʒ��Ϣ��204������� price_mode �ֶΣ�
								price_mode Ϊ "0"ʱ��
								��� = buy_price + price_minchange* buy_pdd
								
								price_mode Ϊ "1"ʱ��
								��� = last_price + price_minchange* buy_pdd
								  
								price_minchange buy_pdd�����ֶ���鿴��Ʒ��ϢӦ�𣬴˹�ʽͬ�������� ����Ʒ��Ϣ�е���ۣ�									
								*/
	
	double		sell_price;		// ����
								/*
								ע�⣬��Զ�Ԫ�����̽��ף��˴������۲������������̿����ۣ����㹫ʽ���£�
								����Ʒ��Ϣ��204������� price_mode �ֶΣ�
								price_mode Ϊ "0"ʱ��
								��� = sell_price + price_minchange* sell_pdd
								
								price_mode Ϊ "1"ʱ��
								��� = last_price + price_minchange* sell_pdd
								  
								��price_minchange sell_pdd�����ֶ���鿴��Ʒ��ϢӦ�𣬴˹�ʽͬ�������� ����Ʒ��Ϣ�е����ۣ�
								*/

	CString		quote_time;		// ����ʱ��

}T_RespQuote;

// Ӧ������
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

// ����ֹӯֹ��
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
	CString	account;		// �˺�
	CString	hold_id;		// �ֲֵ���
	double	stop_loss;		// ֹ���
	double	stop_profit;	// ֹӯ��
	CString	entrust_bs;		// ������� "B"-�� "S"-��
	CString	stock_code;		// ��Ʒ����
	double	entrust_price;	// ί�м۸�
	CString	trade_flag;		// �µ���ʶ "1"-�޸��޼۵�  "2"-����ֹӯֹ��
	CString	user_session_id;		// session_id	
	CString	user_session_info;		// session_info	
	CString	mid_code;				// mid
};

typedef CClientRespError CClientRespSetStopLP;


// ȡ������ֹӯֹ��
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
	CString	account;				// �˺�
	CString	hold_id;				// �ֲֵ���
	CString	cancel_profitloss_type;	// ����ֹ��ֹӯ�۸���� "1"-����ֹ��� "2"-����ֹӯ�� "3"-����ֹ��ֹӯ��
	CString	stock_code;				// ��Ʒ����
	CString	entrust_no;				// ֹӯ/ֹ��� ��ֹӯʱ��ֹӯ�ţ���ֹ��ʱ��ֹ��ţ�ֹӯֹ��ŴӲ�ֲ���ϸ��505��Ӧ��ȡ
	CString	user_session_id;		// session_id	
	CString	user_session_info;		// session_info	
	CString	mid_code;				// mid																	
};

typedef CClientRespError CClientRespCancelStopLP;

// �޸�����
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
	
	CString account;			// �˺�
	CString	password_type;		// �������ͣ�"0"-�������룬"1"-�绰���룬"2"-�ʽ����룩
	CString	password;			// ԭ����
	CString	new_password;		// ������
	CString	user_session_id;	// session_id�ӵ�¼Ӧ���л�ȡ
	CString	user_session_info;	// session_info�ӵ�¼Ӧ���л�ȡ
	CString	mid_code;			// mid�� ��ѯ�ͻ��˺���Ϣ Ӧ���л�ȡ
};

// �޸�����ذ�
typedef CClientRespError CClientRespModifyPwd;

// ��Է�����ԱID ��506��
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

	CString account;			// �˺�
	CString	user_session_id;	// session_id�ӵ�¼Ӧ���л�ȡ
	CString	user_session_info;	// session_info�ӵ�¼Ӧ���л�ȡ
	CString	mid_code;			// mid�� ��ѯ�ͻ��˺���Ϣ Ӧ���л�ȡ
};

// ��Է�����ԱID�ذ�
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

	CString other_id;	// �Է�����ԱID
	CString other_name;	// �Է�����Ա����
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