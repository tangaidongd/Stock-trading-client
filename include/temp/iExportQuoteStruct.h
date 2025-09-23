#ifndef _IEXPORTQUOTESTRUCT_H_
#define _IEXPORTQUOTESTRUCT_H_

// �Ƹ���ܿͻ����ṩ���ⲿ����dll�����鵼���ӿ�  - �ṹ�嶨��
#pragma pack(push, 1)

// ע�������ռ�
// ���г���char�ִ�����utf8���룬��������codeһ�㲻�������ģ����Կ��Ե���asciiʹ��
namespace cfjdExport
{
	typedef unsigned long data32;

	enum{
		MAX_EXPORT_CODE_SIZE		= 32
	};
	enum{
		MAX_EXPORT_NAME_SIZE		= 32
	};
	enum{
		MAX_PRICEVP_COUNT = 20
	};

	// ��Ʒ��������
	enum E_ExportDataType
	{
		EEDTPrice = 1,	// �۸���������
		EEDTKLine = 2,	// K����������
		EEDTTick  = 4,	// �ֱ���������
	};

	// ������������������ʱ��Ҫ��ķ�Χ����
	enum E_ExportReqRangeType
	{
		EERRTRangeBE,	// 1ֵ��2ֵΪʱ��
		EERRTRangeBC,	// 1ֵΪʱ�䣬2ֵΪ����ĸ���	
		EERRTRangeEC,	// 1ֵΪʱ�䣬2ֵΪǰ��ĸ���
	};

	// ������������������ʱ��ָ����������ݷ�Χ
	typedef struct _ExportReqDataRange
	{
		int		iRangeType; // E_ExportReqRangeType ��Χ���ͣ�Ӱ�쿪ʼ�ͽ�����ֵ�Ķ���

		struct BE 
		{
			data32	uBeginTime;		// ��ʼʱ��(��)
			data32	uEndTime;		// ����ʱ��(��)
		};
		struct BC
		{
			data32	uBeginTime;		// ��ʼʱ��(��)
			data32	uFollowCount;	// ָ��ʱ��������K��
		};
		struct CE
		{
			data32	uEndTime;		// ����ʱ��(��)
			data32	uFrontCount;	// ָ��ʱ��ǰ������K��
		};
		union
		{
			BE		stBE;
			BC		stBC;
			CE		stCE;
		} unRange;		// ʵ�ʷ�Χ
	}T_ExportReqDataRange;

	// ֪ͨ�ͻ��˰���ĳ��������������ʱ��ָʾ��ǰ���ڸ����ݵķ�Χ
	typedef struct _ExportDataRange
	{
		data32		uBeginTime;		// ���ݿ�ʼʱ��(��)
		data32		uEndTime;		// ���ݽ���ʱ��(��)
		data32		uCount;			// ���ݹ��ж�����
	}T_ExportDataRange;

	// ��ƷΨһ��ʶ
	typedef struct _ExportMerchKey
	{
		int		iMarket;							// �����г�id
		char	szMerchCode[MAX_EXPORT_CODE_SIZE];	// ��Ʒ�������
	}T_ExportMerchKey;

	//////////////////////////////////////////////////////////////////////////
	// �г�����
	// �г�����
	enum E_ExportMarketType
	{
		EEMTFuturesCn = 0,		// �����ڻ�	
		EEMTStockCn,				// ����֤ȯ
		EEMTMony,				// �����Ŀ
		EEMTExp,					// ָ����Ŀ
		EEMTStockHk,				// �۹���Ŀ
		EEMTFuturesForeign,		// �����ڻ�
			
		// 
		EEMTWarrantCn,			// ����Ȩ֤
		EEMTWarrantHk,			// ���Ȩ֤
			
		//
		EEMTFutureRelaMonth,		// ������ڻ�
		EEMTFutureSpot,			// �ֻ�
		EEMTExpForeign,			// ����ָ��
		EEMTFutureGold,			// �ƽ��ڻ�
	};

	enum E_ExportMarketStatus
	{
		EEMSUnknown = 0,		// δ֪״̬
		EEMSIniting,			// ���ڳ�ʼ��(����?)
		EEMSOpen,				// �ѿ���
		EEMSClose,				// ������
	};

	// �г���Ϣ�еĿ��սṹ��
	typedef struct _ExportMarketOpenCloseTime 
	{
		int		iOpenMinute;		// ��ʽ���Է���Ϊ��λ�� �����00:00�ֵ�ֵ
		int		iCloseMinute;
	}T_ExportMarketOpenCloseTime;

	// �г���Ϣ
	typedef struct _ExportMarketInfo
	{
		int		iBigMarket;	// ���г�
		int		iMarket;	// �г�
		int		iTimeZone;	// �г�����ʱ��
		int		iMarketType;	// �г����� E_ExportMarketType ����

		char	szBourseCode[MAX_EXPORT_CODE_SIZE];	// �����ڽ��������г�����
		char	szEnName[MAX_EXPORT_NAME_SIZE];	// Ӣ������
		char	szCnName[MAX_EXPORT_NAME_SIZE];	// ��������

		int		iTimeInitializePerDay;		// ÿ���ʼ��ʱ�䡣��ʽ���Է���Ϊ��λ�� �����00:00�ֵ�ֵ
											// �ͻ���ÿ�춼��Ҫ��ϵͳ����Ʒ�б�����ʼ���� 
											// ��ʱ������ÿ�γ�ʼ����ʱ�䣨��������ʱ�䣩�� ���ڹ��ڹ��У� ��ʱ�伴�����Ͼ��ۡ�ʱ��

		int		iOpenCloseTimeCount;		// ��Ч�����̶θ���
		T_ExportMarketOpenCloseTime	astOpenCloseTimes[12];	// ÿ��Ŀ�����ʱ��Σ���Ч���� iOpenCloseTimeCount
		

		int		iRecentTradingDayCount;		// ��Ч������������ݸ���
		data32	aiRecentTradingDay[20];		// ��λ: time_t �룬������Ľ����պ�ǰ�Ľ�����(���������,Ȼ��������10��������)

		int		iSubMerchCount;				// ��������Ʒ�ĸ���
	}T_ExportMarketInfo, *LPT_ExportMarketInfo;

	// ��Ʒ��Ϣ
	typedef struct _ExportMerchInfo
	{
		T_ExportMerchKey	stMerchKey;		// ��Ʒ��ʶ
		
		char	szBourseCode[MAX_EXPORT_CODE_SIZE];	// �����ڽ������Ĵ���(���״���)
		char	szEnName[MAX_EXPORT_NAME_SIZE];	// Ӣ������
		char	szCnName[MAX_EXPORT_NAME_SIZE];	// ��������
		
		int		iSaveDec;		// ����С����λ
	}T_ExportMerchInfo, *LPT_ExportMerchInfo;

	//////////////////////////////////////////////////////////////////////////
	// �۸����ݲ���
	// ����������������
	typedef struct _ExportPriceVolume
	{
		float			fPrice;
		float			fVolume;
	}T_ExportPriceVolume;


	// ����
	typedef struct _ExportQuotePrice
	{
		T_ExportMerchKey	stMerchKey;		// ��Ʒ��ʶ
		
		data32	lTime;			// ʱ��
		data32	lMilliSecond;	// ����

		float	fLastClose;		// ���ռ�
		float	fLastAvg;		// �����

		float	fNow;			// ���¼�
		float	fOpen;			// ����
		float	fHigh;			// ��߼�
		float	fAvg;			// ƽ����
		float	fLow;			// ��ͼ�
		
		float	fCurVolume;		// ����
		float	fVolume;		// �ɽ���	
		float	fAmount;		// �ܳɽ���

		float	fHoldPrev;		// ǰ�ֲ�
		float	fCurHold;		// ����
		float	fHold;			// �ֲ���
		
		float	fVolumeRate;	// ����
		float	fBuyVolume;		// ����(���պϼ�), ����(����۳ɽ�), �ڿ���ʱҪ���
		float	fSellVolume;	// ����(���պϼ�), ����(�����۳ɽ�), �ڿ���ʱҪ���

		T_ExportPriceVolume astBuyVP[MAX_PRICEVP_COUNT];		// ���� ������
		T_ExportPriceVolume astSellVP[MAX_PRICEVP_COUNT];		// ���� ������
				
		float	fPE;					// ��ӯ��
		float	fTradeRate;				// ������
		float	fRiseRate;				// ����(���¼ۺ����5�������̼۵ı���)
		float	fMoneyIn;				// �ʽ�����

		float	fSellAmount;			// ���̳ɽ���	
		int		iSeason;				// ����	
	}T_ExportQuotePrice, *LPT_ExportQuotePrice;

	//////////////////////////////////////////////////////////////////////////
	// K�߲���
	// K������
	enum E_ExportKLineType
	{
		EEKTUnknown = 0,		// δ֪����
		EEKTMin,	// ����K��
		EEKT5Min,	// 5����
		EEKT60Min,	// 60����
		EEKTDay,	// ��K��
		EEKTMonth,	// ��K��

		EEKT10Min = 10,		// 10������
		EEKT15Min,			// 15������
		EEKTWeek,			// ����
	};

	// K������
	typedef struct _ExportKLineUnit
	{
		data32	lTime;		// ʱ��
		float	fOpen;		// ����
		float	fHigh;		// ���
		float	fLow;		// ���
		float	fClose;		// ����
		float	fVolume;	// �ɽ���
		float	fAmount;	// �ɽ���
		float	fHold;		// �ֲ�
		float	fAvg;		// �����
		unsigned short	usUpCount;		// ���Ǽ���
		unsigned short	usDownCount;	// �µ�����
	}T_ExportKLineUnit;

	//////////////////////////////////////////////////////////////////////////
	// �ֱʲ���
	// �ֱ�����
	enum E_ExportTickType
	{
		EETTUnknown  = 0,	// ������
		EETTBuy		= 1,	// ���
		EETTSell	= 2,	// ����
		EETTTrade	= 4,	// �ɽ���
	};
	enum E_ExportTickTradeKind
	{
		EETTKUnKnown = 0,		// ����
		EETTKBuy,				// ����۳ɽ�,����
		EETTKSell,				// �����۳ɽ�,����
	};

	// �ֱ�
	typedef struct _ExportTickUnit
	{
		data32	lTime;			// ʱ��
		data32	lMilliSecond;	// ����(��10λ)(��6λ��ʾ����������ͬ����ʱ������)
		float	fPrice;		// �۸�
		float	fVolume;	// ���ʳɽ���
		float	fAmount;	// ���ʳɽ���
		float	fHold;		// �ֲܳ���
		int		iTickType;	// E_ExportTickType���� 
		int		iTradeKind;	// E_ExportTickTradeKind���� �����̱��
	}T_ExportTickUnit;
}

#pragma pack(pop)
#endif //!_IEXPORTQUOTESTRUCT_H_