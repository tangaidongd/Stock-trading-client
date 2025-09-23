#ifndef _PLUG_IN_STRUCT_H_
#define _PLUG_IN_STRUCT_H_


#include "sharestruct.h"
#include "InfoExport.h"


// ���ģ�������, ��Ӧ����
typedef enum E_CommTypePlugIn
{
	ECTPIReqMerchIndex = 1000,		// �����������
	ECTPIRespMerchIndex,

	ECTPIReqBlock,					// ����������
	ECTPIRespBlock,

	ECTPIReqMerchSort,				// ������Ʒ����
	ECTPIRespMerchSort,

	ECTPIReqBlockSort,				// ����������
	ECTPIRespBlockSort,

	ECTPIReqMainMonitor,			// �������
	ECTPIRespMainMonitor,

	ECTPIReqTickEx,					// �ֱ�ͳ��
	ECTPIRespTickEx,

	ECTPIReqHistoryTickEx,			// ��ʷ�ֱ�ͳ��
	ECTPIRespHistoryTickEx,

	ECTPIReqChooseStock,			// ����ѡ�ɼ���
	ECTPIRespChooseStock,

	ECTPIReqDapanState,				// �������״̬
	ECTPIRespDapanState,

	ECTPIReqChooseStockStatus,		// ����ѡ��״̬
	ECTPIRespChooseStockStatus,
	
	ECTPIReqAddPushMerchIndex,		// ���Ӹ�������
	ECTPIRespAddPushMerchIndex,

	ECTPIReqUpdatePushMerchIndex,	// ͬ����������
	ECTPIRespUpdatePushMerchIndex,
	
	ECTPIReqAddPushBlock,			// ���Ӱ����������
	ECTPIRespAddPushBlock,

	ECTPIReqUpdatePushBlock,		// ͬ�������������
	ECTPIRespUpdatePushBlock,
	
	ECTPIReqAddPushMainMonitor,		// ���������������
	ECTPIRespAddPushMainMonitor,

	ECTPIReqUpdatePushMainMonitor,	// ͬ�������������
	ECTPIRespUpdatePushMainMonitor,
	
	ECTPIReqAddPushTickEx,			// ���ӷֱ�����
	ECTPIRespAddPushTickEx,

	ECTPIReqUpdatePushTickEx,		// ͬ���ֱ�����
	ECTPIRespUpdatePushTickEx,
	
	ECTPIReqAddPushChooseStock,		// ����ѡ�ɼ�������
	ECTPIRespAddPushChooseStock,

	ECTPIReqUpdatePushChooseStock,	// ͬ��ѡ�ɼ�������
	ECTPIRespUpdatePushChooseStock,
	
	ECTPIReqDeletePushMerchData,	// ɾ��������Ʒ����
	ECTPIRespDeletePushMerchData,

	ECTPIReqDeletePushBlockData,	// ɾ�����Ͱ������
	ECTPIRespDeletePushBlockData,

	ECTPIReqDeletePushChooseStock,	// ɾ������ѡ������
	ECTPIRespDeletePushChooseStock,
	
	ECTPIRespPushFlag,				// ����,ͬ��,ɾ�����͵Ļذ�(��Ӧ���������Ƿ�ɹ�, ���Ǿ�������)

	ECTPIReqPeriodTickEx,			// ��������ڵķֱ�
	ECTPIRespPeriodTickEx,          // �����ڵķֱʻظ�

	ECTPIReqPeriodMerchSort,        // ��������ڵ���Ʒ����
	ECTPIRespPeriodMerchSort,        // �����ڵ���Ʒ���лظ�

	ECTPIReqAddPushMerchIndexEx3,        // ��������ڵ���Ʒ����
	ECTPIReqAddPushMerchIndexEx5,        // ��������ڵ���Ʒ����
	ECTPIReqAddPushMerchIndexEx10,        // ��������ڵ���Ʒ����

	ECTPIRespAddPushMerchIndexEx3,        // ��������ڵ���Ʒ����
	ECTPIRespAddPushMerchIndexEx5,        // ��������ڵ���Ʒ����
	ECTPIRespAddPushMerchIndexEx10,        // ��������ڵ���Ʒ����

	ECTPIReqShortMonitor,                 // ������߼��
	ECTPIRespShortMonitor,                // ���߼�ػذ�

	ECTPIReqAddPushShortMonitor,          // ���Ӷ��߼������
	ECTPIRespAddPushShortMonitor,          // ���Ӷ��߼������

	ECTPIRespDelPushShortMonitor,          // ɾ�����߼������

	ECTPIReqDKMoney,						// ���Ӷ���ʽ���������
	ECTPIRespDKMoney,						// ���Ӷ���ʽ����ݻظ�

	ECTPIReqCRTEStategyChooseStock,			// ѡ�ɼ�����չ
	ECTPIRespCRTEStategyChooseStock,		// ѡ�ɼ�����չ�ذ�

	ECTPIReqAddPushCRTEStategyChooseStock,	 // ����ѡ�ɼ�����չ����
	ECTPIRespAddPushCRTEStategyChooseStock,

	ECTPIReqMainMasukura,					 // ������������
	ECTPIRespMainMasukura,					 // �������ֻذ�
	
	ECTPIReqAddPushMainMasukura,			 // ����������������
	ECTPIRespAddPushMainMasukura,			

	//ECTPIReqAddPushCRTEStategyChooseStock,		// ����ѡ�ɼ�����չ����
	//ECTPIRespAddPushCRTEStategyChooseStock,


	ECTPIReqCount,

}E_CommTypePlugIn;

// �������е��ֶ�
typedef enum E_ReportSortPlugIn
{
	ERSPIBegin   = 100,

	ERSPIExBigBuyVol,		// ����������
	ERSPIExBigSellVol,		// ����������

	ERSPIBigBuyVol,			// ��������
	ERSPIBigSellVol,		// ��������

	ERSPIMidBuyVol,			// �е�������
	ERSPIMidSellVol,		// �е�������

	ERSPISmallBuyVol,		// С��������
	ERSPISmallSellVol,		// С��������

	ERSPIMnSBuyVol,			// ��С��������
	ERSPIMnSSellVol,		// ��С��������

	ERSPIExBigBuyAmount,	// ���������
	ERSPIExBigSellAmount,	// ����������

	ERSPIBigBuyAmount,		// �������
	ERSPIBigSellAmount,		// ��������

	ERSPIMidBuyAmount,		// �е������
	ERSPIMidSellAmount,		// �е�������

	ERSPISmallBuyAmount,	// С�������
	ERSPISmallSellAmount,	// С��������

	ERSPIExBigNetAmount,	// ���󵥾���
	ERSPIBigNetAmount,		// �󵥾���
	ERSPIMidNetAmount,		// �е�����
	ERSPISmallNetAmount,	// С������

	ERSPITotalBuyAmount,	// �����ܶ�
	ERSPITotalSellAmount,	// �����ܶ�

	ERSPIExBigChange,		// ���󵥻�����
	ERSPIBigChange,			// �󵥻�����
	ERSPIBigBuyChange,		// ����󵥻�����

	ERSPITradeCounts,		// �ɽ�����
	ERSPIAmountPerTrade,	// ÿ�ʽ��
	ERSPIStockPerTrade,		// ÿ�ʹ���
	ERSPIBuyRate,			// �������

	ERSPIShort,				// ����
	ERSPIMid,				// ����

	ERSPIExBigNetBuyVolume,	// ���󵥾�������

	ERSPIEnd,
}E_ReportSortEx;

// ���������ֶ�
typedef enum E_BlockSort
{
	EBSBegin = 200,
	EBSAvgRise,			// ���Ƿ�
	EBSWeightRise,		// Ȩ�Ƿ�
	EBSChange,			// ������
	EBSPeRatio,			// ��ӯ��

	EBSEnd,
}E_BlockSort;

// ����ѡ�ɵ����� (��ͨ��������Ͷ������)
typedef enum E_ChooseStockType
{
	// ��Щ����������ĳ����Ʒ��ĳЩ�ֶ�ֵ
	
	ECSTChooseStock = 0x0001,	// ѡ�ɵ��ֶ�[����, ����]
	ECSTMainMonitor = 0x0002,	// �������
	ECSTTickEx		= 0x0004,	// �ֱ�ͳ��

	// ��Щ����������һ����Ʒ�ļ���, �����ʱ��ֻҪ������, ��������Ʒ�޹�

	ECSTShort		= 0x0008,	// ������Ʒ�༯��
	ECSTMid			= 0x0010,	// ������Ʒ�༯��
	ECSTMrjx		= 0x0020,	// ���뾫ѡ����
	ECSTDpcl		= 0x0040,	// ���̲��Լ���
	ECSTJglt		= 0x0080,	// ������������
	ECSTXpjs		= 0x0100,	// ����ϴ�̼���
	ECSTZjzt		= 0x0200,	// �λ���ͣ����
	ECSTNsql		= 0x0400,	// ����ǿׯ����
	ECSTCdft		= 0x0800,	// ������������
// 	ECSTZlcd		= 0x0020,	// ս�Գ��� ����
// 	ECSTBdcd		= 0x0040,	// ���γ��� ����
// 	ECSTCdft		= 0x0080,	// �������� ����
// 	ECSTJdxgDuo		= 0x0100,	// ���ѡ�ɶ� ����
// 	ECSTShortKong		= 0x0200,	// ���߿� ����
// 	ECSTMidKong		= 0x0400,	// ���߿� ����
// 	ECSTZlzKong		= 0x0800,	// ս������ ����


	// �������
	ECSTBlock		= 0x1000,	// �������


	ECSTSlxs		=0x2000,	// ������ˮ ����
	ECSTHffl		=0x4000,	// �ط���� ����
	ECSTShjd		=0x8000,	// �����۶� ����

	// ����ѡ�ɱ��
// 	ECSTBdzKong		= 0x2000,	// ��������
// 	ECSTCmhtKong	= 0x4000,	// ����ص���
// 	ECSTJdxgKong	= 0x8000,	// ���ѡ�ɿ�

	ECSTShortMonitor	= 0x10000,	// ���߼������ɾ��

	ECSTKtxj		= 0x00010000,	// ��ͷ�������
	ECSTZdsd		= 0x00020000,	// ���ж������
	ECSTDxqs		= 0x00040000,	// ����ǿ�Ʋ���
	ECSTQzhq		= 0x00080000,	// ǿ�ߺ�ǿ����
	ECSTBdcz		= 0x00100000,	// ���β�������
	
	ECSTMainMasukura = 0x00200000,	// ��������
	
}E_PlugInPushType;

// ѡ�ɵ�״̬ ( ���Ⱥ�˳���Ӧ����, ȦȦ���ʲô��. )
enum E_StockState
{
	ESS0 = 0,
	ESS1,
	ESS2,
	ESS3,

	ESSCount,
};

// ����ţ��״̬
enum E_DpnxState
{
	EDS0 = 0,
	EDS1,
	EDS2,
	EDS3,
	EDS4,

	EDSCount,
};

//ѡ��״̬
enum E_ChooseStockState
{
	ECSSNONE = 0x00,	// ��ûѡ����Ʊ
	ECSS1 = 0x01,		// һ������-��ͣ�ȷ�
	ECSS2 = 0x02,		// ����ֱ��-��������
	ECSS3 = 0x04,		// ������ˮ-ϴ�̽���
	ECSS4 = 0x08,		// ��������-����ǿ��
	ECSS5 = 0x10,		// ��ׯ��-����ǿ��
	ECSS6 = 0x20,		// �ط����-��������
	ECSS7 = 0x40,		// �����۶�-ս��ѡ��

	ECSS8	= 0x80,		// �ط����-��ͷ����
	ECSS9	= 0x100,	// ��ׯ��-���ж���
	ECSS10	= 0x200,	// һ������-����ǿ��
	ECSS11	= 0x400,	// ����ֱ��-ǿ�ߺ�ǿ
	ECSS12	= 0x800,	// ������ˮ-���β���
};

// ����ѡ�ɵ�״̬�ذ��ṹ
typedef struct T_RespMerchIndex
{ 
	T_RespMerchIndex()
	{
		m_uiMarket = INT_MIN;
		m_usShort = EDSCount;
		m_usMid = EDSCount;
		m_fCapticalFlow = 0.0f;
		//
		m_fAllNetAmount = 0.0f;
		m_fExBigBuyAmount = 0.0f;
		m_fExBigSellAmount = 0.0f;
		m_fExBigNetAmount = 0.0f;
		m_fExBigNetAmountProportion = 0.0f;
		m_fExBigNetTotalAmount = 0.0f;
		m_fExBigNetTotalAmountProportion = 0.0f;
		m_fBigBuyAmount = 0.0f;
		m_fBigSellAmount = 0.0f;
		m_fBigNetAmount = 0.0f;
		m_fBigNetAmountProportion = 0.0f;
		m_fBigNetTotalAmount = 0.0f;
		m_fBigNetTotalAmountProportion = 0.0f;
		m_fMidBuyAmount = 0.0f;
		m_fMidSellAmount = 0.0f;
		m_fMidNetAmount = 0.0f;
		m_fMidNetAmountProportion = 0.0f;
		m_fMidNetTotalAmount = 0.0f;
		m_fMidNetTotalAmountProportion = 0.0f;
		m_fSmallBuyAmount = 0.0f;
		m_fSmallSellAmount = 0.0f;
		m_fSmallNetAmount = 0.0f;
		m_fSmallNetAmountProportion = 0.0f;
		m_fSmallNetTotalAmount = 0.0f;
		m_fSmallNetTotalAmountProportion = 0.0f;
	}
	int32	m_uiMarket;					// �г�
	CString	m_StrCode;					// ����
	int32	m_usShort;					// ����״̬	(E_StockStateֵ)
	int32	m_usMid;					// ����״̬
	float	m_fCapticalFlow;			// �ʽ�����

	//
	float m_fAllNetAmount;			// ������(��Ԫ)

	//
	float m_fExBigBuyAmount;		// ��������(��Ԫ)
	float m_fExBigSellAmount;		// ��������(��Ԫ)
	float m_fExBigNetAmount;		// ���󵥾���(��Ԫ)
	float m_fExBigNetAmountProportion;		// ���󵥾���ռ��
	float m_fExBigNetTotalAmount;			// �����ܶ�(��Ԫ)
	float m_fExBigNetTotalAmountProportion;	// �����ܶ�ռ��

	//
	float m_fBigBuyAmount;		// ������(��Ԫ)
	float m_fBigSellAmount;		// ������(��Ԫ)
	float m_fBigNetAmount;		// �󵥾���(��Ԫ)
	float m_fBigNetAmountProportion;		// �󵥾���ռ��
	float m_fBigNetTotalAmount;				// ���ܶ�(��Ԫ)
	float m_fBigNetTotalAmountProportion;	// ���ܶ�ռ��

	//
	float m_fMidBuyAmount;		// �е�����(��Ԫ)
	float m_fMidSellAmount;		// �е�����(��Ԫ)
	float m_fMidNetAmount;		// �е�����(��Ԫ)
	float m_fMidNetAmountProportion;		// �е�����ռ��
	float m_fMidNetTotalAmount;				// �е��ܶ�(��Ԫ)
	float m_fMidNetTotalAmountProportion;	// �е��ܶ�ռ��

	//
	float m_fSmallBuyAmount;	// С������(��Ԫ)
	float m_fSmallSellAmount;	// С������(��Ԫ)
	float m_fSmallNetAmount;	// С������(��Ԫ)
	float m_fSmallNetAmountProportion;		// С������ռ�� 
	float m_fSmallNetTotalAmount;			// С���ܶ�(��Ԫ)
	float m_fSmallNetTotalAmountProportion;	// С���ܶ�ռ��

}T_RespMerchIndex;

typedef struct T_RespMainMasukura
{
	T_RespMainMasukura()
	{					
		m_uiMarket = 0;;						
		m_StrCode = L"";		

		m_fMasukuraProportion = 0.0;			
		m_iRanked = 0;						
		m_fRise = 0.0;	
		m_f2DaysMasukuraProportion = 0.0;	
		m_i2DaysRanked = 0;					
		m_f2DaysRise = 0.0;	
		m_f3DaysMasukuraProportion = 0.0;	
		m_i3DaysRanked = 0;					
		m_f3DaysRise = 0.0;					
		m_f5DaysMasukuraProportion = 0.0;;		
		m_i5DaysRanked = 0;					
		m_f5DaysRise = 0.0;;					
		m_f10DaysMasukuraProportion = 0.0;;	
		m_i10DaysRanked = 0;				
		m_f10DaysRise = 0.0;;					
	}

	int32		m_uiMarket;						// �г�
	CString 	m_StrCode;						// ����

	float		m_fMasukuraProportion;			// ��������ռ��
	int32		m_iRanked;						// ��������
	float		m_fRise;						// �����Ƿ�

	float		m_f2DaysMasukuraProportion;		// 2������ռ��
	int32		m_i2DaysRanked;					// 2������
	float		m_f2DaysRise;					// 2���Ƿ�

	float		m_f3DaysMasukuraProportion;		// 3������ռ��
	int32		m_i3DaysRanked;					// 3������
	float		m_f3DaysRise;					// 3���Ƿ�

	float		m_f5DaysMasukuraProportion;		// 5������ռ��
	int32		m_i5DaysRanked;					// 5������
	float		m_f5DaysRise;					// 5���Ƿ�

	float		m_f10DaysMasukuraProportion;	// 10������ռ��
	int32		m_i10DaysRanked;				// 10������
	float		m_f10DaysRise;					// 10���Ƿ�

}T_RespMainMasukura;

// ������ݵĻذ��ṹ
typedef struct T_LogicBlock
{
	T_LogicBlock()
	{
		m_iBlockId = INT_MIN;
		m_iRiseCounts = 0;
		m_iFallCounts = 0;
		m_iRiseDays   = 0;
		m_fAvgRise = m_fWeightRise = m_fTotalAmount = m_fChange = m_fPEratio = m_fMarketRate = m_fCapticalFlow = m_fMerchRise =
			m_fRiseRate = m_fMainNetVolume =m_fMainAmount = m_fVolumeRate = m_fAllVolume = m_fAllValue = m_fCircValue =0.0f;
	}
	int32	m_iBlockId;
	float	m_fAvgRise;			// ���Ƿ�
	float	m_fWeightRise;		// Ȩ�Ƿ�
	
	float	m_fTotalAmount;		// �ܳɽ���
	float	m_fChange;			// ������
	float	m_fPEratio;			// ��ӯ��
	CString	m_StrMerchName;		// ���ǹ�Ʊ
	float   m_fMerchRise;		// ���ǹ�Ʊ�Ƿ�
	int32   m_iRiseCounts;		// �ǹ�
	int32   m_iFallCounts;		// ����
	float	m_fMarketRate;		// �г���
	float	m_fCapticalFlow;	// �ʽ�����
	int32   m_iRiseDays;		// ��������
	float	m_fRiseRate;		// ����
	float	m_fMainNetVolume;	// ��������
	float	m_fMainAmount;		// �������
	float	m_fVolumeRate;		// ����
	float	m_fAllVolume;		// ����
	float	m_fAllValue;		// ����ֵ
	float	m_fCircValue;		// ��ͨ��ֵ
}T_LogicBlock;

// ������ص�����ṹ(���߼������ʹ�øýṹ)
typedef struct T_ReqMainMonitor
{
	int32			m_iCount;		// ������ʱ�俪ʼ��ǰ�������������
	
	int32			m_iBlockId;		// ���ID
	CString			m_StrMerchCode;	// ��Ʒ���� (�������ֵ, �ͷ���������������Ʒ������)
}T_ReqMainMonitor;

// ������صĻذ��ṹ
typedef struct T_RespMainMonitor
{
public:
	T_RespMainMonitor& operator=(const T_RespMainMonitor& Src)
	{
		if ( &Src == this )
		{
			return *this;
		}

		m_Time	  = Src.m_Time;
		m_fPrice  = Src.m_fPrice;
		m_fCounts = Src.m_fCounts;
		m_uBuy    = Src.m_uBuy;

		return *this;
	}

	long	m_Time;			// ʱ��
	float	m_fPrice;		// �۸�
	float	m_fCounts;		// ����
	u8		m_uBuy;			// �������� or ��������
	
}T_RespMainMonitor;

// ������ص���������
typedef std::vector<T_RespMainMonitor> MonitorArray;

typedef struct T_MainMonitorNode
{
	CMerchKey		m_MerchKey;
	MonitorArray	m_aMainMonitor;
}T_MainMonitorNode;

typedef std::vector<T_MainMonitorNode> mapMainMonitor;


// ���߼�صĻذ��ṹ
typedef struct T_RespShortMonitor
{
public:
	T_RespShortMonitor& operator=(const T_RespShortMonitor& Src)
	{
		if ( &Src == this )
		{
			return *this;
		}

		m_Time	  = Src.m_Time;
		m_fPrice  = Src.m_fPrice;
		m_fCounts = Src.m_fCounts;
		m_uBuy    = Src.m_uBuy;
		m_uMonitorType = Src.m_uMonitorType;
		m_fIncRate = Src.m_fIncRate;

		return *this;
	}

	bool32 operator==(const T_RespShortMonitor& Src) const
	{
		return m_Time == Src.m_Time
		&& m_fPrice  == Src.m_fPrice
		&& m_fCounts == Src.m_fCounts
		&& m_uBuy == Src.m_uBuy
		&& m_uMonitorType == Src.m_uMonitorType
		&& m_fIncRate == Src.m_fIncRate;
	}

	long	m_Time;			// ʱ��
	float	m_fPrice;		// �۸�
	float	m_fCounts;		// ����
	u8		m_uBuy;			// �������� or ��������
	u8      m_uMonitorType; // �������
	float   m_fIncRate;     // �ǵ���

}T_RespShortMonitor;

// ������ص���������
typedef std::vector<T_RespShortMonitor> ShortMonitorArray;

typedef struct T_ShortMonitorNode
{
	CMerchKey		m_MerchKey;
	ShortMonitorArray	m_aShortMonitor;
}T_ShortMonitorNode;

typedef std::vector<T_ShortMonitorNode> mapShortMonitor;

// �ֱʶ������ݵĽṹ
typedef struct T_TickEx
{
	int32	m_iMarketId;			// �г�
	CString	m_StrMerchCode;			// ����
	
	float	m_fExBigBuyVol;			// ��������������ɣ�
	float	m_fExBigSellVol;		// ��������������ɣ�
	
	float	m_fBigBuyVol;			// ������������ɣ�
	float	m_fBigSellVol;			// ������������ɣ�
	
	float	m_fMidBuyVol;			// �е�����������ɣ�
	float	m_fMidSellVol;			// �е�����������ɣ�
	
	float	m_fSmallBuyVol;			// С������������ɣ�
	float	m_fSmallSellVol;		// С������������ɣ�
	
	float	m_fMnSBuyVol;			// ��С������������ɣ�
	float	m_fMnSSellVol;			// ��С������������ɣ�
	
	float	m_fExBigBuyAmount;		// ����������Ԫ��
	float	m_fExBigSellAmount;		// �����������Ԫ��
	
	float	m_fBigBuyAmount;		// ��������Ԫ��				
	float	m_fBigSellAmount;		// ���������Ԫ��			

	float	m_fMidBuyAmount;		// �е�������Ԫ��
	float	m_fMidSellAmount;		// �е��������Ԫ��
	
	float	m_fSmallBuyAmount;		// С��������Ԫ��
	float	m_fSmallSellAmount;		// С���������Ԫ��
	
	float	m_fAllBuyAmount;		// �����ܶ��Ԫ��
	float	m_fAllSellAmount;		// �����ܶ��Ԫ��
	
	float	m_fAmountPerTrans;		// ÿ�ʽ���Ԫ��
	float	m_fStocksPerTrans;		// ÿ�ʹ������ɣ�
	
	float	m_fPriceNew;			// ����
	float	m_fExBigChange;			// ���󵥻�����
	float	m_fBigChange;			// �󵥻�����
	float	m_fBigBuyChange;		// ����󵥻�����
	uint32	m_uiTradeTimes;			// �ɽ�����
	
	float	m_fBuyRate;				// �������

	T_TickEx()
	{	
		m_iMarketId			= INT_MIN;
		m_StrMerchCode		= L"";

		m_fExBigBuyVol		= 0.;	
		m_fExBigSellVol		= 0.;	
		
		m_fBigBuyVol        = 0.;	
		m_fBigSellVol       = 0.;	
		
		m_fMidBuyVol        = 0.;	
		m_fMidSellVol       = 0.;	
		
		m_fSmallBuyVol      = 0.;	
		m_fSmallSellVol     = 0.;	
		
		m_fMnSBuyVol        = 0.;	
		m_fMnSSellVol		= 0.;	
		
		m_fExBigBuyAmount   = 0.;	
		m_fExBigSellAmount  = 0.;	
		
		m_fBigBuyAmount     = 0.;	
		m_fBigSellAmount    = 0.;	
		
		m_fMidBuyAmount     = 0.;	
		m_fMidSellAmount    = 0.;	
		
		m_fSmallBuyAmount   = 0.;	
		m_fSmallSellAmount  = 0.;	
		
		m_fAllBuyAmount     = 0.;	
		m_fAllSellAmount    = 0.;	
		
		m_fAmountPerTrans   = 0.;	
		m_fStocksPerTrans   = 0.;	
		
		m_fPriceNew         = 0.;	
		m_fExBigChange      = 0.;	
		m_fBigChange        = 0.;	
		m_fBigBuyChange     = 0.;	
		m_uiTradeTimes      = 0;	
		
		m_fBuyRate			= 0.;		
	}

}T_TickEx;

typedef struct T_ReqPushMerchData
{
	int32	m_iMarket;		// �г�
	CString m_StrCode;		// ����
	u32		m_uType;		// �������Ϳ��Ի�ķ�ʽ��ʾ���
	
}T_ReqPushMerchData;

typedef struct T_ReqPushBlockData
{
	int32	m_iBlockId;				// �г�	
	E_ChooseStockType m_eType;		// �������Ϳ��Ի�ķ�ʽ��ʾ���

}T_ReqPushBlockData;

////////////////////////////////////////////////////////////////////////////////
class CMmiCommBasePlugIn;

// ����һ���������
DATAINFO_DLL_EXPORT CMmiCommBasePlugIn*	 NewCopyCommPlugInReqObject(CMmiCommBasePlugIn *pCommBase);

// �ж����������Ƿ�ͬ����ͬ����
DATAINFO_DLL_EXPORT bool32				IsSamePlugInReqObject(const CMmiCommBasePlugIn *pCommSrc, const CMmiCommBasePlugIn *pCommDst);		


/************************************************************************************************************************************/
// ���ģ����������
class DATAINFO_DLL_EXPORT CMmiCommBasePlugIn : public CMmiCommBase
{
public:
	CMmiCommBasePlugIn() { m_eCommType = ECTReqPlugIn; }
	virtual ~CMmiCommBasePlugIn() {}
	virtual CString GetSummary() { return L""; }

public:
	E_CommTypePlugIn m_eCommTypePlugIn;
};

/**********************************************************************************************************/
// ������Ʒ��ѡ���ֶ� [����, ����] ֵ. ������ÿ 5 ���Ӽ���һ��.
class DATAINFO_DLL_EXPORT CMmiReqMerchIndex : public CMmiCommBasePlugIn
{
public:
	CMmiReqMerchIndex() { m_eCommTypePlugIn = ECTPIReqMerchIndex; }
	virtual CString	GetSummary();
	
	//
	CMmiReqMerchIndex(const CMmiReqMerchIndex& Src);
	CMmiReqMerchIndex& operator= (const CMmiReqMerchIndex& Src);

public:
	int32			m_iMarketId;		// �г����
	CString			m_StrMerchCode;		// ��Ʒ����

	CArray<CMerchKey, CMerchKey&>	m_aMerchMore;	// ������һ����Ʒʱ�� �������ñ���(Ϊ����һ����Ʒʱ��д��)
};

// ��Ʒ��ѡ������Ļذ�
class DATAINFO_DLL_EXPORT CMmiRespMerchIndex : public CMmiCommBasePlugIn
{
public:
	CMmiRespMerchIndex() { m_eCommType = ECTRespPlugIn; m_eCommTypePlugIn = ECTPIRespMerchIndex; }
	virtual CString	GetSummary();

public:
	CArray<T_RespMerchIndex, T_RespMerchIndex> m_aMerchIndexList;	
};

// ����������������
class DATAINFO_DLL_EXPORT CMmiReqMainMasukura : public CMmiCommBasePlugIn
{
public:
	CMmiReqMainMasukura() { m_eCommTypePlugIn = ECTPIReqMainMasukura; }
	virtual CString	GetSummary();

	//
	CMmiReqMainMasukura(const CMmiReqMainMasukura& Src);
	CMmiReqMainMasukura& operator= (const CMmiReqMainMasukura& Src);

public:
	int32			m_iMarketId;		// �г����
	CString			m_StrMerchCode;		// ��Ʒ����

	CArray<CMerchKey, CMerchKey&>	m_aMerchMore;	// ������һ����Ʒʱ�� �������ñ���(Ϊ����һ����Ʒʱ��д��)
};

// ������������Ļذ�
class DATAINFO_DLL_EXPORT CMmiRespMainMasukura : public CMmiCommBasePlugIn
{
public:
	CMmiRespMainMasukura() { m_eCommType = ECTRespPlugIn; m_eCommTypePlugIn = ECTPIRespMainMasukura; }
	virtual CString	GetSummary();

public:
	CArray<T_RespMainMasukura, T_RespMainMasukura> m_aMainMasukuraList;	
};

// ���������� ( ���Ƿ�, Ȩ�Ƿ�, ���ǹ�Ʊ�ȵ�...)
class DATAINFO_DLL_EXPORT CMmiReqLogicBlock : public CMmiCommBasePlugIn
{
public:
	CMmiReqLogicBlock() { m_eCommTypePlugIn = ECTPIReqBlock; }
	virtual CString	GetSummary(){return CString();}
	
	CMmiReqLogicBlock(const CMmiReqLogicBlock& Src);
	CMmiReqLogicBlock& operator=(const CMmiReqLogicBlock& Src);

public:
	int32			m_iLogicBlockId;		// �г����

	//
	CArray<int32, int32>	m_aBlockMore;			
};

// �ذ�
class DATAINFO_DLL_EXPORT CMmiRespLogicBlock : public CMmiCommBasePlugIn
{
public:
	CMmiRespLogicBlock() { m_eCommType = ECTRespPlugIn; m_eCommTypePlugIn = ECTPIRespBlock; };
	virtual CString	GetSummary();
	
public:
	CArray<T_LogicBlock, T_LogicBlock&> m_aBlockData;
};

// ������Ʒ����
class DATAINFO_DLL_EXPORT CMmiReqMerchSort : public CMmiCommBasePlugIn
{
public:
	CMmiReqMerchSort() { m_eCommTypePlugIn = ECTPIReqMerchSort; }
	virtual CString GetSummary() {return CString();}

	CMmiReqMerchSort(const CMmiReqMerchSort& Src);
	CMmiReqMerchSort& operator=(const CMmiReqMerchSort& Src);

public:
	int32			m_iMarketId;		// �г����
	bool32			m_bDescSort;		// ����,����
	int32			m_iStart;			// ��ʼλ��
	int32			m_iCount;			// ��Ʒ����
	
	E_ReportSortEx	m_eReportSortType;	// ��������
};

// ������Ʒ��������
class DATAINFO_DLL_EXPORT CMmiReqPeriodMerchSort : public CMmiCommBasePlugIn
{
public:
	CMmiReqPeriodMerchSort() { m_eCommTypePlugIn = ECTPIReqPeriodMerchSort; }
	~CMmiReqPeriodMerchSort() {}
	virtual CString GetSummary() {return CString();}

	CMmiReqPeriodMerchSort(const CMmiReqPeriodMerchSort& Src);
	CMmiReqPeriodMerchSort& operator=(const CMmiReqPeriodMerchSort& Src);

public:
	int32			m_iMarketId;		// �г����
	bool32			m_bDescSort;		// ����,����
	int32			m_iStart;			// ��ʼλ��
	int32			m_iCount;			// ��Ʒ����
    uint8           m_uiPeriod;         // ����
	E_ReportSortEx	m_eReportSortType;	// ��������
};

// Ӧ����Ʒ���а�����
class DATAINFO_DLL_EXPORT CMmiRespMerchSort : public CMmiCommBasePlugIn
{
public:
	CMmiRespMerchSort() { m_eCommType = ECTRespPlugIn; m_eCommTypePlugIn = ECTPIRespMerchSort; }
	virtual ~CMmiRespMerchSort(){};
	
public:
	virtual CString GetSummary();
	
public:
	int32			m_iMarketId;				// �г����
	
	E_ReportSortEx	m_eReportSortType;
	bool32			m_bDescSort;				// �������У�
	int32			m_iStart;
	
	// ��Ʒ�б�
	CArray<CMerchKey, CMerchKey&> m_aMerchs;	// ��Ʒ5����������
};

// Ӧ������ڵ���Ʒ���а�����
class DATAINFO_DLL_EXPORT CMmiRespPeriodMerchSort : public CMmiCommBasePlugIn
{
public:
	CMmiRespPeriodMerchSort() { m_eCommType = ECTRespPlugIn; m_eCommTypePlugIn = ECTPIRespPeriodMerchSort; }
	virtual ~CMmiRespPeriodMerchSort(){};

public:
	virtual CString GetSummary() { return CString(); }

public:
	int32			m_iMarketId;				// �г����

	E_ReportSortEx	m_eReportSortType;
	bool32			m_bDescSort;				// �������У�
	int32			m_iStart;
	uint8           m_iPeriod;

	// ��Ʒ�б�
	CArray<CMerchKey, CMerchKey&> m_aMerchs;	// ��Ʒ5����������
};

// ����������
class DATAINFO_DLL_EXPORT CMmiReqBlockSort : public CMmiCommBasePlugIn
{
public:
	CMmiReqBlockSort() { m_eCommTypePlugIn = ECTPIReqBlockSort; }
	virtual CString GetSummary() {return CString();}

	CMmiReqBlockSort(const CMmiReqBlockSort& Src);
	CMmiReqBlockSort& operator=(const CMmiReqBlockSort& Src);

public:
	int32			m_iMarketId;		// �г����
	bool32			m_bDescSort;		// ����,����
	int32			m_iStart;			// ��ʼλ��
	int32			m_iCount;			// ��Ʒ����
	
	E_BlockSort		m_eBlockSortType;	// ��������
};	

class DATAINFO_DLL_EXPORT CMmiRespBlockSort : public CMmiCommBasePlugIn
{
public:
	CMmiRespBlockSort() { m_eCommType = ECTRespPlugIn; m_eCommTypePlugIn = ECTPIRespBlockSort; }
	virtual CString GetSummary();
	
public:
	int32			m_iBlockId;			// ID
	bool32			m_bDescSort;		// ����,����
	int32			m_iStart;			// ��ʼλ��
	int32			m_iCount;			// ��Ʒ����
	
	E_BlockSort		m_eBlockSortType;	// ��������

	CArray<int32, int32>	m_aBlockIDs;	// �źõİ��ID
};
	
// ��������������� 
class DATAINFO_DLL_EXPORT CMmiReqMainMonitor: public CMmiCommBasePlugIn
{
public:
	CMmiReqMainMonitor() { m_eCommTypePlugIn = ECTPIReqMainMonitor; }
	virtual CString	GetSummary(){return CString();}
	
	CMmiReqMainMonitor(const CMmiReqMainMonitor& Src);
	CMmiReqMainMonitor& operator=(const CMmiReqMainMonitor& Src);

public:
	int32			m_iCount;		// ������ʱ�俪ʼ��ǰ�������������
	
	int32			m_iBlockId;		// ���ID
	CString			m_StrMerchCode;	// ��Ʒ���� (�������ֵ, �ͷ���������������Ʒ������)
		
	//
	CArray<T_ReqMainMonitor, T_ReqMainMonitor&>	m_aReqMore;	// ����ͬʱ��������Ʒ	
};

// ������ػذ�
class DATAINFO_DLL_EXPORT CMmiRespMainMonitor : public CMmiCommBasePlugIn
{
public:
	CMmiRespMainMonitor() { m_eCommType = ECTRespPlugIn; m_eCommTypePlugIn = ECTPIRespMainMonitor; };
	CMmiRespMainMonitor(const CMmiRespMainMonitor& Src);	
	CMmiRespMainMonitor& operator=(const CMmiRespMainMonitor& Src);

	virtual CString	GetSummary();

public:
	mapMainMonitor	m_mapMainMonitorData;
};

// ������߼������ 
class DATAINFO_DLL_EXPORT CMmiReqShortMonitor: public CMmiCommBasePlugIn
{
public:
	CMmiReqShortMonitor() { m_eCommTypePlugIn = ECTPIReqShortMonitor; }
	virtual CString	GetSummary(){return CString();}

	CMmiReqShortMonitor(const CMmiReqShortMonitor& Src);
	CMmiReqShortMonitor& operator=(const CMmiReqShortMonitor& Src);

public:
	int32			m_iCount;		// ������ʱ�俪ʼ��ǰ�������������

	int32			m_iBlockId;		// ���ID
	CString			m_StrMerchCode;	// ��Ʒ���� (�������ֵ, �ͷ���������������Ʒ������)

	//
	CArray<T_ReqMainMonitor, T_ReqMainMonitor&>	m_aReqMore;	// ����ͬʱ��������Ʒ	
};

// ���߼�ػذ�
class DATAINFO_DLL_EXPORT CMmiRespShortMonitor : public CMmiCommBasePlugIn
{
public:
	CMmiRespShortMonitor() { m_eCommType = ECTRespPlugIn; m_eCommTypePlugIn = ECTPIRespShortMonitor; }
	CMmiRespShortMonitor(const CMmiRespShortMonitor& Src);	
	CMmiRespShortMonitor& operator=(const CMmiRespShortMonitor& Src);

	virtual CString	GetSummary() {return CString();}

public:
	mapShortMonitor	m_mapShortMonitorData;
};

// ����ֱ�ͳ��
class DATAINFO_DLL_EXPORT CMmiReqTickEx : public CMmiCommBasePlugIn
{
public:
	CMmiReqTickEx::CMmiReqTickEx() { m_eCommTypePlugIn = ECTPIReqTickEx; }
	virtual CString	GetSummary(){return CString();}
	
	CMmiReqTickEx(const CMmiReqTickEx& Src);
	CMmiReqTickEx& operator=(const CMmiReqTickEx& Src);

public:
	int32		m_iMarketId;
	CString		m_StrMerchCode;
	
	CArray<CMerchKey, CMerchKey>	m_aMerchMore;
};

// ��������ڵķֱ�ͳ��
class DATAINFO_DLL_EXPORT CMmiReqPeriodTickEx : public CMmiCommBasePlugIn
{
public:
	CMmiReqPeriodTickEx::CMmiReqPeriodTickEx() { m_eCommTypePlugIn = ECTPIReqPeriodTickEx; }
	virtual CString	GetSummary(){return CString();}

	CMmiReqPeriodTickEx(const CMmiReqPeriodTickEx& Src);
	CMmiReqPeriodTickEx& operator=(const CMmiReqPeriodTickEx& Src);

public:
	int32		m_iMarketId;
	CString		m_StrMerchCode;
	uint8       m_uiPeriod;                // ����

	CArray<CPeriodMerchKey, CPeriodMerchKey>	m_aMerchMore;
};

// �ֱ�ͳ�ƻذ�
class DATAINFO_DLL_EXPORT CMmiRespTickEx : public CMmiCommBasePlugIn	
{
public:
	CMmiRespTickEx::CMmiRespTickEx() { m_eCommType = ECTRespPlugIn; m_eCommTypePlugIn = ECTPIRespTickEx; }
	virtual CString	GetSummary();
	
public:
	CArray<T_TickEx, T_TickEx&> m_aTickEx;
};

// ���շֱ�ͳ�ƻذ�
class DATAINFO_DLL_EXPORT CMmiRespPeriodTickEx : public CMmiCommBasePlugIn	
{
public:
	CMmiRespPeriodTickEx::CMmiRespPeriodTickEx() { m_eCommType = ECTRespPlugIn; m_eCommTypePlugIn = ECTPIRespPeriodTickEx; }
	virtual CString	GetSummary();

public:
	uint8 m_iPeriod; 
	CArray<T_TickEx, T_TickEx&> m_aTickEx;
};

// ������ʷ�ֱ�
class DATAINFO_DLL_EXPORT CMmiReqHistoryTickEx : public CMmiCommBasePlugIn
{
public:
	CMmiReqHistoryTickEx() { m_eCommTypePlugIn = ECTPIReqHistoryTickEx; }
	virtual CString GetSummary() { return L""; }
	
	CMmiReqHistoryTickEx(const CMmiReqHistoryTickEx& Src);
	CMmiReqHistoryTickEx& operator= (const CMmiReqHistoryTickEx& Src);

public:
	int32		m_iMarketId;
	CString		m_StrMerchCode;
};

// ��ʷ�ֱʻذ�
class DATAINFO_DLL_EXPORT CMmiRespHistoryTickEx : public CMmiCommBasePlugIn
{
public:
	CMmiRespHistoryTickEx() { m_eCommType = ECTRespPlugIn; m_eCommTypePlugIn = ECTPIRespHistoryTickEx; }
	virtual CString GetSummary() { return L""; }

public:
	CArray<T_TickEx, T_TickEx&> m_aTickEx;
};

// ����ѡ�ɼ���
class DATAINFO_DLL_EXPORT CMmiReqChooseStock : public CMmiCommBasePlugIn
{
public:
	CMmiReqChooseStock::CMmiReqChooseStock() { m_eCommTypePlugIn = ECTPIReqChooseStock; }
	virtual CString	GetSummary(){return CString();}

	CMmiReqChooseStock(const CMmiReqChooseStock& Src);
	CMmiReqChooseStock& operator=(const CMmiReqChooseStock& Src);

public:
	u16		m_uTypes;			// ѡ������ E_ChooseStockType���� s
};

// Ӧ��
class DATAINFO_DLL_EXPORT CMmiRespChooseStock : public CMmiCommBasePlugIn
{
public:
	CMmiRespChooseStock::CMmiRespChooseStock() { m_eCommType = ECTRespPlugIn; m_eCommTypePlugIn = ECTPIRespChooseStock; }
	virtual CString	GetSummary();
	
public:
	u16		m_uTypes;			// ѡ������ E_ChooseStockType���� s

	CArray<CMerchKey, CMerchKey>	m_aMerchs;	// ��Ʒ����
};



// ����ѡ�ɼ�����չ
class DATAINFO_DLL_EXPORT CMmiReqCRTEStategyChooseStock : public CMmiCommBasePlugIn
{
public:
	CMmiReqCRTEStategyChooseStock::CMmiReqCRTEStategyChooseStock() { m_eCommTypePlugIn = ECTPIReqCRTEStategyChooseStock; }
	virtual CString	GetSummary(){return CString();}

	CMmiReqCRTEStategyChooseStock(const CMmiReqCRTEStategyChooseStock& Src);
	CMmiReqCRTEStategyChooseStock& operator=(const CMmiReqCRTEStategyChooseStock& Src);

public:
	u32		m_uTypes;			// ѡ������ E_ChooseStockType���� s
};

// ����ѡ�ɼ�����չ Ӧ��
class DATAINFO_DLL_EXPORT CMmiRespCRTEStategyChooseStock : public CMmiCommBasePlugIn
{
public:
	CMmiRespCRTEStategyChooseStock::CMmiRespCRTEStategyChooseStock() { m_eCommType = ECTRespPlugIn; m_eCommTypePlugIn = ECTPIRespCRTEStategyChooseStock; }
	virtual CString	GetSummary();

public:
	u32		m_uTypes;			// ѡ������ E_ChooseStockType���� s

	CArray<CMerchKey, CMerchKey>	m_aMerchs;	// ��Ʒ����
};


// �������״̬
class DATAINFO_DLL_EXPORT CMmiReqDapanState : public CMmiCommBasePlugIn
{
public:
	CMmiReqDapanState() { m_eCommTypePlugIn = ECTPIReqDapanState; }
	virtual CString	GetSummary(){return CString();}

	CMmiReqDapanState(const CMmiReqDapanState& Src);
	CMmiReqDapanState& operator=(const CMmiReqDapanState& Src);
};

// �ذ�
class DATAINFO_DLL_EXPORT CMmiRespDapanState : public CMmiCommBasePlugIn
{
public:
	CMmiRespDapanState() { m_eCommType = ECTRespPlugIn; m_eCommTypePlugIn = ECTPIRespDapanState; }
	virtual CString	GetSummary();

public:
	E_DpnxState	m_eDapanState;					// ����״̬
}; 

// ����ѡ��״̬
class DATAINFO_DLL_EXPORT CMmiReqChooseStockStatus : public CMmiCommBasePlugIn
{
public:
	CMmiReqChooseStockStatus() { m_eCommTypePlugIn = ECTPIReqChooseStockStatus; }
	virtual CString	GetSummary(){return CString();}

	CMmiReqChooseStockStatus(const CMmiReqChooseStockStatus& Src);
	CMmiReqChooseStockStatus& operator=(const CMmiReqChooseStockStatus& Src);
};

// �ذ�
class DATAINFO_DLL_EXPORT CMmiRespChooseStockStatus : public CMmiCommBasePlugIn
{
public:
	CMmiRespChooseStockStatus() { m_eCommType = ECTRespPlugIn; m_eCommTypePlugIn = ECTPIRespChooseStockStatus; }
	virtual CString	GetSummary();

public:
	u32	m_uStatus;					// ����״̬
}; 



// �������ʽ�����
class DATAINFO_DLL_EXPORT CMmiReqDKMoney : public CMmiCommBasePlugIn
{
public:
	CMmiReqDKMoney() { m_eCommTypePlugIn = ECTPIReqDKMoney; }
	virtual CString	GetSummary(){return CString();}

	CMmiReqDKMoney(const CMmiReqDKMoney& Src);
	CMmiReqDKMoney& operator=(const CMmiReqDKMoney& Src);
public:
	int32		m_iMarketID;	
	CString		m_StrMerchCode;		// ��Ʒ
	u16			m_uType;			// �������� E_ChooseStockType �� "|" ����
};

// �ذ�
class DATAINFO_DLL_EXPORT CMmiRespDKMoney : public CMmiCommBasePlugIn
{
public:
	CMmiRespDKMoney() { m_eCommType = ECTRespPlugIn; m_eCommTypePlugIn = ECTPIRespDKMoney; }
	virtual CString	GetSummary() {return CString();};

public:
	float m_fITBF;//�����������ʽ�
	float m_fITSF;//�����������ʽ�
	float m_fMultSide;//�෽ռ��
	float m_fEmpSide;//�շ�ռ��
}; 



// �������͵�����Ʒ��ص���������, ���� 	ECSTChooseStock ECSTMainMonitor ECSTTickEx
class DATAINFO_DLL_EXPORT CMmiReqPushPlugInMerchData : public CMmiCommBasePlugIn
{
public:
	CMmiReqPushPlugInMerchData(){ m_eCommTypePlugIn = ECTPIReqAddPushMerchIndex; }
	virtual CString	GetSummary(){return CString();}

	CMmiReqPushPlugInMerchData(const CMmiReqPushPlugInMerchData& Src);
	CMmiReqPushPlugInMerchData& operator=(const CMmiReqPushPlugInMerchData& Src);

public:
	int32		m_iMarketID;			
	CString		m_StrMerchCode;		// ��Ʒ
	u32			m_uType;			// �������� E_ChooseStockType �� "|" ����

	CArray<T_ReqPushMerchData, T_ReqPushMerchData&>	m_aReqMore;
};

// �������Ͱ������
class DATAINFO_DLL_EXPORT CMmiReqPushPlugInBlockData : public CMmiCommBasePlugIn
{
public:
	CMmiReqPushPlugInBlockData(){ m_eCommTypePlugIn = ECTPIReqAddPushBlock;  m_eType = ECSTBlock; }
	virtual CString	GetSummary(){return CString();}
	
	CMmiReqPushPlugInBlockData(const CMmiReqPushPlugInBlockData& Src);
	CMmiReqPushPlugInBlockData& operator=(const CMmiReqPushPlugInBlockData& Src);

public:
	int32	m_iBlockID;
	E_ChooseStockType		m_eType;			// �������� ECSTBlock

	CArray<T_ReqPushBlockData, T_ReqPushBlockData&> m_aReqMore;
};

// ������������ѡ�ɵļ��� ECSTShort ECSTMid ECSTMrjx ECSTDpcl ECSTJglt ECSTXpjs ECSTZjzt ECSTNsql ECSTCdft
class DATAINFO_DLL_EXPORT CMmiReqPushPlugInChooseStock: public CMmiCommBasePlugIn
{
public:
	CMmiReqPushPlugInChooseStock() { m_eCommTypePlugIn = ECTPIReqAddPushChooseStock; }
	virtual CString	GetSummary(){return CString();}

	CMmiReqPushPlugInChooseStock(const CMmiReqPushPlugInChooseStock& Src);
	CMmiReqPushPlugInChooseStock& operator=(const CMmiReqPushPlugInChooseStock& Src);

public:
	//
	u32			m_uType;			//  ѡ�����͵� "|" ����
};

// ͬ��������Ʒ�������, �õ�ǰ��Щ��Ʒ�滻ԭ����������Ʒ
class DATAINFO_DLL_EXPORT CMmiReqUpdatePushPlugInMerchData : public CMmiCommBasePlugIn
{
public:
	CMmiReqUpdatePushPlugInMerchData(E_CommTypePlugIn eType)
	{
		if ( eType != ECTPIReqUpdatePushMerchIndex && eType != ECTPIReqUpdatePushTickEx && eType != ECTPIReqUpdatePushMainMonitor )
		{
			ASSERT(0);
		}

		m_eCommType = ECTReqPlugIn; 
		m_eCommTypePlugIn = eType;
	}

	virtual CString	GetSummary(){return CString();}
	
	CMmiReqUpdatePushPlugInMerchData(const CMmiReqUpdatePushPlugInMerchData& Src);
	CMmiReqUpdatePushPlugInMerchData& operator=(const CMmiReqUpdatePushPlugInMerchData& Src);
	
public:
	CArray<CMerchKey, CMerchKey> m_aMerchMore;
};

// ͬ�����Ͱ����������
class DATAINFO_DLL_EXPORT CMmiReqUpdatePushPlugInBlockData : public CMmiCommBasePlugIn
{
public:
	CMmiReqUpdatePushPlugInBlockData(){ m_eCommTypePlugIn = ECTPIReqUpdatePushBlock; }
	virtual CString	GetSummary(){return CString();}
	
	CMmiReqUpdatePushPlugInBlockData(const CMmiReqUpdatePushPlugInBlockData& Src);
	CMmiReqUpdatePushPlugInBlockData& operator=(const CMmiReqUpdatePushPlugInBlockData& Src);

public:	
	CArray<int32, int32>	m_aBlockIDs;		// ���id
};

// ͬ������ѡ�ɼ��ϵ�������������һ��
typedef CMmiReqPushPlugInChooseStock CMmiReqUpdatePushPlugInChooseStock;

// ɾ����Ʒ��������
class DATAINFO_DLL_EXPORT CMmiReqRemovePushPlugInMerchData : public CMmiCommBasePlugIn
{
public:
	CMmiReqRemovePushPlugInMerchData(E_CommTypePlugIn eType) { m_eCommTypePlugIn = ECTPIReqDeletePushMerchData; }
	virtual CString	GetSummary(){return CString();}

	CMmiReqRemovePushPlugInMerchData(const CMmiReqRemovePushPlugInMerchData& Src);
	CMmiReqRemovePushPlugInMerchData& operator=(const CMmiReqRemovePushPlugInMerchData& Src);

public:	
	E_ChooseStockType				m_eType;
	CArray<CMerchKey, CMerchKey>	m_aMerchs;
};

// ɾ�������������
class DATAINFO_DLL_EXPORT CMmiReqRemovePushPlugInBlockData : public CMmiCommBasePlugIn
{
public:
	CMmiReqRemovePushPlugInBlockData() { m_eCommTypePlugIn = ECTPIReqDeletePushBlockData; m_eType = ECSTBlock; }
	virtual CString	GetSummary(){return CString();}
	
	CMmiReqRemovePushPlugInBlockData(const CMmiReqRemovePushPlugInBlockData& Src);
	CMmiReqRemovePushPlugInBlockData& operator=(const CMmiReqRemovePushPlugInBlockData& Src);
	
public:		
	E_ChooseStockType		m_eType;
	CArray<int32, int32>	m_aBlockIDs;
};

// ɾ��ѡ������
class DATAINFO_DLL_EXPORT CMmiReqRemovePushPlugInChooseStock: public CMmiCommBasePlugIn
{
public:
	CMmiReqRemovePushPlugInChooseStock() { m_eCommTypePlugIn = ECTPIReqDeletePushChooseStock; }
	virtual CString	GetSummary(){return CString();}
	
	CMmiReqRemovePushPlugInChooseStock(const CMmiReqRemovePushPlugInChooseStock& Src);
	CMmiReqRemovePushPlugInChooseStock& operator=(const CMmiReqRemovePushPlugInChooseStock& Src);
	
public:		
	E_ChooseStockType	m_eType;
};

// �����������Ļذ�
class DATAINFO_DLL_EXPORT CMmiRespPlugInPushFlag : public CMmiCommBasePlugIn
{
public:
	CMmiRespPlugInPushFlag() { m_eCommType = ECTRespPlugIn; m_eCommTypePlugIn = ECTPIRespPushFlag; }
	virtual CString	GetSummary() { return L""; }

	UINT m_uiTag;
};

#endif // _PLUG_IN_STRUCT_H_