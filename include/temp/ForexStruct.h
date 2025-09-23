#include <afxtempl.h>
#include <string>

using namespace std;

#ifndef _FOREX_STRUCT_H_
#define _FOREX_STRUCT_H_

// Э������
typedef enum E_ForexInfo
{
	EFIReqOrder				= 7001,			// �����µ�(���ݽṹ���� 9513)
	EFIRespOrder			= 7002,			// �µ��ذ�(���ݽṹ���� 9513)
	EFIRespBargain			= 7003,			// ��ʱ�����׳ɹ�(���ݽṹ���� 9512)
	EFIRespModifyEntry		= 7004,			// �޸Ĺҵ�(���ݽṹ���� 9513)
	EFIRespDelEntry			= 7005,			// ɾ���ҵ�
	EFIReqHistoryOrder		= 7900,			// ��ѯ��ʷ�ɽ���¼

	EFILogin				= 8000,			// ��¼
	EFILogOut				= 8001,			// �ǳ�
	EFIHeart				= 9002,			// ����
	EFIAccount				= 9501,			// �ʻ���Ϣ
	EFICurrency				= 9505,			// ������Ϣ
	EFIDefaultMerch			= 9511,			// Ĭ����Ʒ��Ϣ
	EFIBargain				= 9512,			// �ɽ���Ϣ
	EFIOrder				= 9513,			// ί�е���Ϣ
	EFIHistoryOrder			= 9514,			// ���㵥��Ϣ
	EFIGroup				= 9515,			// ����Ϣ
	EFIBase					= 9516,			// ������Ϣ
	EFIQuotePush			= 9518,			// ��������
	EFIBroadcast			= 9524,			// ������Ϣ
	EFIError				= 9999,			// ������Ϣ

}E_ForexReqType;


// ��������
enum E_ErrorCode
{
	// �Զ���Ĵ���
	ErrNone			= 0,	// û�д���
	ErrInitalFailed,		// ��ʼ��ʧ��
	ErrSslIniFailed,		// SSL ��ʼ��ʧ��
	ErrAddressInValid,		// ��ַ��Ϣ����
	ErrConnectFailed,		// connect ʧ��
	ErrSslConnectFailed,	// SSL connect ʧ��
	ErrThreadFailed,		// Thread Create Failed
	
	// ���������صĴ���
	Err1001 = 1001,			//	��������ȷ                                  
	Err1002,                //	�۸���ȷ                                  
	Err1003,                //	δ֪����                                    
	Err1004,                //	���벻����                                  
	Err1005,                //	�������                                    
	Err1006,                //	��ͬ�ʺŴ������ն˵�¼�����ѱ�ǿ���˳�ϵͳ  
	Err1007,                //	�޲���Ȩ��                                  
	Err1008,                //	�ǽ���ʱ��                                  
	Err1009,                //	�ʽ���                                    
	Err1010,                //	��Ʒ����ȷ                                  
	Err1011,                //	ί����������                                
	Err1012,                //	ί���ѱ�����                                
	Err1013,                //	ί���ѱ���ȫ                                
	Err1014,                //	ί���ѱ�����                                
	Err1015,                //	ί���ѳɽ���                                
	Err1016,                //	ָʾ�Ѿ���Ч                                
	Err1017,                //	ί���ѱ���ȫ                                
	Err1018,                //	ί���ѱ�����                                
	Err1019,                //	ί��������Ч                                
	Err1020,                //	ί���ѳɽ���                                
	Err1021,                //	ί���ѱ�����                                
	Err1022,                //	�������                                    
	Err1023,                //	������̫��                                  
	Err1024,                //	���벻��Ϊ��                                
	Err1025,                //	�ͻ����볤��                                
	Err1026,                //	���볤�ȱ���                                
	Err1027,                //	ǰ����������                                
	Err1028,                //	�Ŵ������Ϊ                                
	Err1029,                //	��Ч����ָ��                                
	Err1030,                //	���׷���Ͽ�                                
	Err1031,                //	�ʺű�����                                  
	Err1032,                //	ֹ��۸���ȷ                              
	Err1033,                //	�����۸���ȷ                              
	Err1034,                //	��Ʒ���벻����                              
	Err1035,                //	��ƽ����������ȷ                            
	Err1036,                //	�ǽ���Ʒ��                                  
	Err1037,                //	�г��۸��Ѹı�                              
	Err1038,                //	Ҫƽ�ֵĵ�������                            
	Err1039,                //	Ҫ������ί�е�������                        
	Err1040,                //	����ȷ                                  
	Err1041,                //	δ֪����                                    
	Err1042,                //	��¼�ʺŲ�����                              
	Err1043,                //	������������                              
	Err1044,                //	ƽ�ּ��ʱ����ڹ涨ֵ                      
	Err1045,                //	��ֹ�����µ�                                
	Err1046,                //	�гֲ�ʱ��ֹ�޸Ľ��ն�/���ö�               
	Err1047,                //	���ն�ܴ������ö�                        
	Err1048,                //	���ö���ֵ���Ϸ�                            
	Err1049,                //	��ȡ����                                
	Err1050,                //	���������ֲ��޶�                            
	Err1051,                //	ϵͳ��æ,                //���Ժ����в���                     
	Err1052,                //	ֹ��۸񳬳�ָ����Χ����ο�����            
	Err1053,                //	ֹӮ�۸񳬳�ָ����Χ����ο�����            
	Err1054,                //	�Ҽ۲��Ϸ�                                  
	Err1055,                //	���鲻ͬ��,                //�ȴ����±���                     
	Err3001 = 3001,                //	��¼�Ѵ���                                  
	Err3002,                //	�ؼ��ֶβ���Ϊ��                            
	Err3003,                //	�г����벻��                                
	Err3004,                //	���Ҵ��벻��                                
	Err3005,                //	��֤�����Ͳ���                              
	Err3006,                //	��Ϣ���Ͳ���                                
	Err3007,                //	�����ѱ��벻��                              
	Err3008,                //	�����𲻶�                                
	Err3009,                //	ί���Ѵ���                                  
	Err3010,                //	��ѯ��¼������                              
	Err5001 = 5001,                //	ͬ�����ó�ʱ                                
	Err5002,                //	Ѱ����������δ�ҵ�����                    
	Err5003,                //	Ѱ����������δ�ҵ�����                    
	Err6001 = 6001,                //	�޸�����ʱ�����벻һ��                      
	Err6002,                //	���볤�ȱ�����6��16λ֮��                   
};

// ʵʱ����
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

// ��������Ϣ
class CServer
{
public:
	CString		m_StrServerAdd;
	DWORD		m_dwPort;
};

// �����Ʒ��
class CForexMerch  
{
public:
	CForexMerch() {}
	virtual ~CForexMerch(){}
 
public:
	CString			m_StrCode;			// 1	����
	int				m_iMerchType;		// 2	��Ʒ���� 9:Exoit,10:Major,11:Minor,12:Metal,13:��ۺ�Լ,14:ָ��
	CString			m_StrUnit;			// 3	��λ, һֱû��ֵ, ��֪����ʲô���͵��ֶ� ...fangz 0720
	int				m_iChargeType;		// 4	����������
	int				m_iDepositType;		// 5	��֤������
	int				m_iInterestType;	// 6	��Ϣ����
	float			m_fContractSize;	// 7	��Լ��С
	float			m_fDepositValue;	// 8	��֤��ֵ
	float			m_fDepositNight;	// 9	��ҹ��֤��ֵ
	float			m_fMinPriceChange;	// 10	��С�۸�䶯
	float			m_fBuyInterest;		// 11	������Ϣֵ	
	float			m_fSellInterest;	// 12	������Ϣֵ	
    float			m_fChargeOpen;		// 13	����������	
	float			m_fChargeClose;		// 14	ƽ��������	
	float			m_fPointDiff;		// 15	���	
	float			m_fPriceLmtPtDiff;	// 16	�޼۵��	
	int				m_iFlagTrade;		// 17	�����ױ�־	
	float			m_fTimesPrice;		// 18	�۸����	ʵ�ʼ۸�=�۸�/�۸����
	float			m_fTimesVolume;		// 19	��������	ʵ����������/��������
	float			m_fMinTradeVol;		// 20	��С��������	
	int				m_iMerchType1;		// 21	��Ʒ����1	1:����Ԫ��Ϊ���ۻ���,2:����Ԫ��Ϊ��������,3:�����̣����ʶ�����Ԫ��Ϊ���ۻ���,4:�����̣����ʶ�����Ԫ��Ϊ�������� ...fangz 0720 ������Ʒ����??
	float			m_fPriceBuy;		// 22	����۸�	
	float			m_fPriceSell;		// 23	�����۸�	
	float			m_fPriceHigh;		// 24	��߼۸�	
	float			m_fPriceLow;		// 25	��ͼ۸�	
	float			m_fPriceOpen;		// 26	���̼۸�	
	float			m_fPricePreClose;	// 27	�������м۸�	
	float			m_fReserver29;		// 28	[�����ֶ�]	
	//float			m_fReserver30;		// 29	[�����ֶ�]	
	CString			m_StrDataSourceMerch;	// 29 ������Դ��Ʒ(��ӽ�������)
	float			m_iMarketID;		// 30	�г�����	
	float			m_fReserver32;		// 31	[�����ֶ�]	
	CString			m_Strcurrency;		// 32	����	
	int				m_iTradeType;		// 33	��������	
	float			m_fMaxMarketVol;	// 34	����г��ɽ�����	
	int				m_iValidBillType;	// 35	�ҵ���Ч������	91��һֱ��Ч��92��������Ч������ֹӮֹ��,93:������Ч(��ֹӮֹ��)��110���ҵ����գ�ֹӮֹ���ܣ�111���ҵ����ܣ�ֹӮֹ����
	int				m_i3DaysCalcType;	// 36	���ռ�Ϣģʽ	120:����,121:��һ,122:�ܶ�,123:����,124:����,125����,126:����
	float			m_afReserver[16];	// 37-52	[�����ֶ�]	
	float			m_iLockDepositType;	// 53	������֤������	
	float			m_fReserver55;		// 54	[�����ֶ�]	
	int				m_iFlagDefault;		// 55	ȱʡ��־	
	CString			m_StrOverDate;		// 56	������	
	float			m_fDepositAlarm;	// 57	��֤�𱨾�ˮƽ	
	float			m_fForceClose;		// 58	ǿ��ƽ��ˮƽ	
	float			m_fReserver60;		// 59	[�����ֶ�]	
	float			m_fBillCharge;		// 60	����	
	//float			m_afReserver2[26];	// 61-86 [�����ֶ�]	
	float			m_afReserver2[14];	// 61-74 [�����ֶ�]	
	int				m_iOrderPriceMaxType;	// 75 �ҵ���������
	int				m_iOrderPriceMinType;	// 76 �ҵ���������
	int				m_iStopLoseMaxType;		// 77 ֹ����������
	int				m_iStopLoseMinType;		// 78 ֹ����������
	int				m_iStopProfitMaxType;	// 79 ֹӯ��������
	int				m_iStopProfitMinType;	// 80 ֹӯ��������
	float			m_fOrderPriceMax;		// 81 �ҵ�����ֵ
	float			m_fOrderPriceMin;		// 82 �ҵ�����ֵ
	float			m_fStopLostMax;			// 83 ֹ������ֵ
	float			m_fStopLoseMin;			// 84 ֹ������ֵ
	float			m_fStopProfitMax;		// 85 ֹӯ����ֵ
	float			m_fStopProfitMin;		// 86 ֹӯ����ֵ
	float			m_fLockDepositValue;// 87	������֤��ֵ
	float			m_fReserve88;		// 88	[�����ֶ�]
	//
	CForexRealtimePrice	m_RealtimePrice;// ʵʱ��������, 9518 ����	
};

// ������Ϣ
class CCurrency
{
public:

	CCurrency(){}
	~CCurrency(){}

public:

	CString		m_StrCode;			// ���Ҵ���
	CString		m_StrName;			// ��������	
	CString		m_StrNameCn;		// ��������
	float		m_fInterestRate;	// ����
	float		m_fExchangeRate;	// �ҽ�����һ���
	int			m_iFlagCurrency;	// ������ұ�־
	float		m_fReserver7;		// �����ֶ�
	float		m_fReserver8;		// �����ֶ�
};	

// ����Ϣ
class CGroup
{
public:

	CGroup(){}
	~CGroup(){}

public:
	CString		m_StrCode;				// �����
	int			m_iState;				// ״̬, 42������43���ᣬ44ֻ����45������
	CString		m_StrCompanyName;		// ��˾����
	CString		m_StrDepositCurrency;	// ��֤�����
	int			m_iTimesDefault;		// ȱʡ����
	float		m_fIncome;				// Ͷ������
	int			m_iHighSecurityFlag;	// �߰�ȫ�����־
	float		m_afReserver[27];		// ʣ�� 27 �������ֶ�
};

// ������Ϣ
class CBroadCast
{
public:

	CBroadCast(){}
	~CBroadCast(){}

public:
	int			m_iNo;					// ��Ϣ���
	float		m_fReserver2;			// �����ֶ�
	float		m_fReserver3;			// �����ֶ�
	CString		m_StrTitle;				// ����
	CString		m_StrAuthor;			// ������
	CString		m_StrDate;				// ����
	CString		m_StrTime;				// ʱ��
	CString		m_StrContent;			// ����
};

// ��������
class CBaseInfo
{
public:

	CBaseInfo(){}
	~CBaseInfo(){}

public:
	// ��һ����¼
	CString		m_StrTraderName;		// 1	����������
	//float		m_afReserver[24];		// 2-25 �����ֶ�
	CString		m_StrReserver1[6];		// 2-7  ����
	CString		m_StrBalanceTimeInDay;	// 8	ÿ�ս���ʱ��
	CString		m_StrReserver2[17];		// 9-25	����

	// �ڶ�����¼
	float		m_afReserver2[4];		// 1-4  �����ֶ�
	CString		m_StrHistoryIP;			// 5	��ʷ����IP
	int			m_iHistoryPort;			// 6	��ʷ���ݶ˿�
	float		m_afReserver3[9];		// 7-15 �����ֶ�
	int			m_iFlagDayCharge;		// 16   ���ս��������ѱ�־
	CString		m_StrTradeDate;			// 17	��������
	float		m_afReserver4[11];		// 18-28�����ֶ�
};

// CAccountInfo �е��ʽ���Ϣ
typedef struct T_FundInfo
{
public:
	double		m_fBalance;				// 1	���	
	float		m_afReserver[5];		// 2	[�����ֶ�]											
	double		m_fDepositUseage;		// 7	ռ�ñ�֤���	
	float		m_fReserver8;			// 8	[�����ֶ�]	
	double		m_fBalancePre;			// 9	�������	
	double		m_fEarnToday;			// 10	����ƽ��ӯ�����	
	double		m_fInToday;				// 11	��������	
	double		m_fOutToday;			// 12	���ճ�����	
	double		m_fFactorageToday;		// 13	����������	
	CString		m_StrCurrencyCode;		// 14	���Ҵ���

}T_FundInfo;

// �ʺ���Ϣ
class CAccountInfo
{
public:

	CAccountInfo(){}
	~CAccountInfo(){}

public:
	// ��һ��
	CString		m_StrUserID;			// �ͻ��ʺ�

	// �ڶ���, �ͻ�������Ϣ
	CString		m_StrPwd;				// 1	����
	CString		m_StrBrokerCode;		// 2	�����˴���
	CString		m_StrAccountName;		// 3	�˻�����
	CString		m_StrAccountNameEn;		// 4	�˻�����(Ӣ��)
	CString     m_StrAddress1;			// 5	��ַ1
	CString     m_StrAddress2;			// 6	��ַ2
	CString     m_StrPhone1;			// 7	�绰1
	CString     m_StrPhone2;			// 8	�绰2
	CString     m_StrMobile;			// 9	�ֻ�����
	CString		m_StrFax;				// 10	����
	int			m_iFlagAllowIn;			// 11	�����µ���־(δ��)
	int			m_iFlagShowCharge;		// 12	��ʾ�����ѱ�־(δ��)
	int			m_iGroupCode;			// 13	�����
	int			m_iTypeAccount;			// 14	�ʺ�����(δ��)
	CString		m_StrCurrencyCodeUnUse;	// 15	���Ҵ���(δ��)
	int			m_iTypeUser;			// 16	�û�����(δ��)
	double		m_fLineOfCredit;		// 17	���ö��
	double		m_fMaxDeposit;			// 18	���֤��
	int			m_iLevelControl;		// 19	���Ƽ���(δ��)
	int			m_iState;				// 20	42������43���ᣬ44ֻ����45������
	float		m_fReserver21;			// 21	�����ֶ�
	long		m_lColor;				// 22	��ɫ(δ��)
	CString		m_StrCity;				// 23	����
	CString		m_StrArea;				// 24	����
	CString		m_StrPostCode;			// 25	��������
	CString		m_StrEmail;				// 26	�����ʼ�
	CString		m_StrPS;				// 27	��ע
	CString		m_StrPaperNum;			// 28	֤������
	CString		m_StrProxyCode;			// 29	�����̴���
	CString		m_StrInvestPwd;			// 30	Ͷ������
	CString		m_StrPhonePwd;			// 31	�绰����
	CString		m_StrOpenDate;			// 32	��������
	CString		m_StrOpenTime;			// 33	����ʱ��
    int			m_iFlagAllowChgPwd;		// 34	�����޸������־
	int			m_iFlagReadOnly;		// 35	ֻ����־
	int			m_iFlagSendReport;		// 36	���ͱ����־
	float		m_fTimesDeposit;		// 37	��֤����
	float		m_fRateTax;				// 38	˰��
	CString		m_StrLogInTime;			// 39	��¼ʱ��
	CString		m_StrLogOutTime;		// 40	�ǳ�ʱ��
    float		m_fReserver41;			// 41	[�����ֶ�]
	int			m_iLogInTimes;			// 42	��¼����
	float		m_fReserver43;			// 43	[�����ֶ�]
	
	// ������ �ʽ���Ϣ
	CArray<T_FundInfo, T_FundInfo&>	m_aFundInfos;

	// ���Ķ� ��Ʒ��Ϣ
	CArray<CForexMerch,	CForexMerch&> m_aMerchInfos;	
	
	// �����
	int			m_iInternalNo;			// �ڲ����
};

// �ɽ���Ϣ
class CBargain
{
public:

	CBargain() {};
	~CBargain(){}

public:
	CString		m_StrUserID;		// 1	�ͻ��ʺ�	
	CString		m_StrMerchCode;		// 2	��Ʒ����	
	CString		m_StrDate;			// 3	�ɽ�����	
	CString		m_StrTime;			// 4	�ɽ�ʱ��	
	int			m_iBargainNo;		// 5	�ɽ������	
	int			m_iColseNo;			// 6	ƽ�ֵ����	
	int			m_iInternalNo;		// 7	�ڲ����	
	CString		m_StrOperatorCode;	// 8	����Ա����	
	CString		m_StrPS;			// 9	��ע	
	int			m_iOperateFlag;		// 10	����	40:����,41:����
	int			m_iCloseFlag;		// 11	ƽ�ֵ���־	
	float		m_fVolume;			// 12	����	
	float		m_fPrice;			// 13	�۸�	
	float		m_fPriceStopFall;	// 14	ֹ��۸�	ί�е��е�ֹ��۸�(��ʾ��)
	float		m_fPriceStopRise;	// 15	ֹӯ�۸�	ί�е��е�ֹӯ�۸�(��ʾ��)
	float		m_fReserver16;		// 16	[�����ֶ�]	
	float		m_fCharge;			// 17	������	
	float		m_fInterest;		// 18	��Ϣ	
	float		m_fReserver19;		// 19	[�����ֶ�]	
	float		m_fBillCharge;		// 20	����	
	float		m_fReserver21;		// 21	[�����ֶ�]	
};

// ί�е���Ϣ	(�ͻ��˷���ί��ָ��������˷���ί��ָ��������Ҳʹ�ñ�ָ��,ͨ��ί�е��������жϽ��д���)
class COrderInfo
{
public:

	COrderInfo(){}
	~COrderInfo(){}

public:
	CString		m_StrUserID;				// 1	�ͻ��ʺ�	
	CString		m_StrMerchCode;				// 2	��Ʒ����	
	CString		m_StrOrderDate;				// 3	ί������	YYYYMMDD
	int			m_iInternalNo;				// 4	�ڲ����	�Զ�
	CString		m_StrOrderTime;				// 5	ί��ʱ��	HHNNSS
	int			m_iOrderNo;					// 6	ί�е����	ί�е����,ϵͳ����ί�е���Ŷ���Ψһ��,ί�е��ɽ����תΪ�ɽ������
	int			m_iCloseNo;					// 7	ƽ�ֵ����	�����ƽ�ֵ�,���ֶ�����Ҫƽ�Ŀ��ֵ���ί�е����
	int			m_iOrderType;				// 8	ί������:
											//				A:�ͻ���ί��ָ��	60:�м��µ�,61:�޼��µ�,62:�м�ƽ�ֵ�,63:�޼�ƽ�ֵ�,64:�޼�����,65:�޼�����,66:ֹ������,67:ֹ������,68:�޸��޼�ƽ�ֵ�,69:ɾ���޼�ƽ�ֵ�,71:�ͻ�������ȡ���м۵�,77:���۵�
											//				B:�������˷���ָ��1	70:�г��۸��Ѹı�(�ͻ�ί��Ϊ�м۵�ָ��ʱ,��Ҫ�Ѵ���ȥ�ļ۸�ͬ�������ϵ����¼۸���Ƚ�,������������涨ֵ,�򷵻ر���־,��ʱ��15�ֶξ��Ƿ������ϵ���������۸�,��Ҫ����ί�е������������ʾ�������ͻ���,����ʾ�ͻ��Ƿ���Ҫ�Դ˼۸��ٴν���ί���걨)
											//				C:�������˷���ָ��2	64,65,66,67(����ͬA),����ί�е��ѱ�����������,���ڵȴ��ɽ��׶�
											//				D:�������˷���ָ��3	69,71:��ί�е��ѱ�����
											//				E:�������˷���ָ��4	ί�е��ѱ��������ܾ�,�ܾ�ԭ�������29�ֶ�(��ϸ�������������)
	CString		m_StrOperatorCode;			// 9	����Ա����	
	int			m_iValidBillType;			// 10	�ҵ���Ч������	91��һֱ��Ч��92��������Ч������ֹӮֹ��,93:������Ч(��ֹӮֹ��)��110���ҵ����գ�ֹӮֹ���ܣ�111���ҵ����ܣ�ֹӮֹ����
	int			m_iBuySell;					// 11	����	40:����,41:����
	int			m_iState;					// 12	״̬	25:��ʼ״̬,26:������,27:ȫ���ɽ�,28:���ֳɽ�,29:����,31:��Ч,32:�ύ��,33:������,34:�ı���,35:ɾ����,36:��Ч��
	int			m_iCloseFlag;				// 13	ƽ�ֵ���־	
	float		m_fVolume;					// 14	����	
	float		m_fPrice;					// 15	�۸�	
	float		m_fVolumeSuccess;			// 16	�ѳɽ�����	
	float		m_fPriceStopFall;			// 17	ֹ��۸�	
	float		m_fPriceStopRise;			// 18	ֹӯ�۸�	
	float		m_fPointDiff;				// 19	������	
	float		m_fReserver20;				// 20	[�����ֶ�]	��0
	float		m_fReserver21;				// 21	[�����ֶ�]	��0
	float		m_fReserver22;				// 22	[�����ֶ�]	��0
	float		m_fReserver23;				// 23	[�����ֶ�]	��0
	float		m_fReserver24;				// 24	[�����ֶ�]	
	float		m_fReserver25;				// 25	[�����ֶ�]	
	float		m_fReserver26;				// 26	[�����ֶ�]	
	float		m_fReserver27;				// 27	[�����ֶ�]	
	CString		m_StrPS;					// 28	��ע	
	//float		m_fReserver29;				// 29	[�����ֶ�]	��0							
	float		m_fPriceInUSD;				// 29	��Ʒ����Ԫ��ʾ�ļ۸�
	float		m_fReserver30;				// 30	[����30]
	float		m_fReserver31;				// 31	[����31]
};

// �ѽ���ƽ�ֵ���Ϣ(��ʷ�ɽ���Ϣ)
class CHistoryOrderInfo
{
public:

	CHistoryOrderInfo(){}
	~CHistoryOrderInfo(){}

public:
	CString		m_StrTradeDate;				// 0	��������	
	CString		m_StrUserID;				// 1	�ͻ��ʺ�	
	CString		m_StrMerchCode;				// 2	��Ʒ����	
	float		m_fVolume;					// 3	����	
	CString		m_StrOpenDate;				// 4	��������	
	CString		m_StrOpenTime;				// 5	����ʱ��	
	int			m_iOpenNo;					// 6	���ֵ����	
	int			m_iOpenInternalNo;			// 7	���ֵ��ڲ����	
	CString		m_StrOpenOperatorCode;		// 8	���ֵ�����Ա����	
	CString		m_StrOpenPS;				// 9	���ֵ���ע	
	int			m_iOpenBuySell;				// 10	���ֵ�����	
											//		A�����׼�¼	40:����,41:����
											//		B��������¼	142:���,143:����,��ʱ21�ֶζ�Ӧ����ĳ������
	float		m_fPriceOpen;				// 11	���ֵ��۸�	
	float		m_fPriceStopRise;			// 12	���ֵ�ֹӮ�۸�	
	float		m_fPriceStopFall;			// 13	���ֵ�ֹ��۸�	
	CString		m_StrCloseDate;				// 14	ƽ������	
	CString		m_StrCloseTime;				// 15	ƽ��ʱ��	
	int			m_iCloseNo;					// 16	ƽ�ֵ����	
	int			m_iCloseInternalNo;			// 17	ƽ�ֵ��ڲ����	
	CString		m_StrCloseOperatorCode;		// 18	ƽ�ֵ�����Ա����	
	CString		m_StrClosePS;				// 19	ƽ�ֵ���ע	
	float		m_fPriceClose;				// 20	ƽ�ּ۸�	
	float		m_fEarn;					// 21	ӯ��	
	float		m_fReserver22;				// 22	[�����ֶ�]	
	float		m_fInterest;				// 23	��Ϣ	
	float		m_fCharge;					// 24	������	
	float		m_fBillCharge;				// 25	����	
};

// ��¼��Ϣ
class CForexLogInfo
{
public:
	CString		m_StrLog;			// ��¼����
	CString		m_StrTime;			// ʱ��
};

#endif	// _FOREX_STRUCT_H_