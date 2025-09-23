#ifndef _MERCH_MANAGER_H_
#define _MERCH_MANAGER_H_

#pragma warning(disable: 4786)

#include "GmtTime.h"
#include "LandMineStruct.h"
#include "InfoExport.h"

//lint -sem(CArray::Add, custodial(1))

class CMerch;
class CBroker;
class CMarket;
class CBreed;
class CMmiBroker;
 
struct T_OpenCloseTime;

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// �г��������г������
///////////////////////////////////////////////
typedef struct T_OpenCloseTime
{
public:
	T_OpenCloseTime()
	{
		m_iTimeOpen		= 0;
		m_iTimeClose	= 0;
	}

public:
	int32			m_iTimeOpen;			// ��ʽ���Է���Ϊ��λ�� �����00:00�ֵ�ֵ
	int32			m_iTimeClose;
}T_OpenCloseTime;

/////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct T_ClientTradeTime
{
public:
	T_ClientTradeTime()
	{
		m_iWeek = -1;
		m_iOpen = -1;
		m_iClose = -1;
		m_aOCTimes.RemoveAll();
	}

	T_ClientTradeTime(const T_ClientTradeTime& stSrc)
	{
		m_iWeek = stSrc.m_iWeek;
		m_iOpen = stSrc.m_iOpen;
		m_iClose= stSrc.m_iClose;

		m_aOCTimes.Copy(stSrc.m_aOCTimes);
	}

	const T_ClientTradeTime& T_ClientTradeTime::operator=(const T_ClientTradeTime& stSrc)
	{
		if (this != &stSrc)
		{
			m_iWeek = stSrc.m_iWeek;
			m_iOpen = stSrc.m_iOpen;
			m_iClose= stSrc.m_iClose;

			m_aOCTimes.Copy(stSrc.m_aOCTimes);
		}

		return *this;
	}

	//
	int32	m_iWeek;	// ���ڼ���ʱ�䣬-6��ʾ�������������죬����ʱ���ʾͨ��ʱ��
	long	m_iOpen;	// ����ʱ��
	long    m_iClose;	// ����ʱ��

	//
	CArray<T_OpenCloseTime, T_OpenCloseTime&> m_aOCTimes;	// ����ʱ���

}T_ClientTradeTime;

/////////////////////////////////////////////////////////////////////////////////////////////////////////

// 
typedef CArray<T_ClientTradeTime, T_ClientTradeTime&> arrClientTradeTime;

typedef struct T_TradeTimeInfo
{
public:
	T_TradeTimeInfo()
	{
		m_iTradeTimeID = -1;
		m_aClientTradeTime.RemoveAll();
	}

	//
	T_TradeTimeInfo(const T_TradeTimeInfo& stSrc)
	{
		m_iTradeTimeID = stSrc.m_iTradeTimeID;
		m_aClientTradeTime.Copy(stSrc.m_aClientTradeTime);
	}

	const T_TradeTimeInfo& T_TradeTimeInfo::operator=(const T_TradeTimeInfo& stSrc)
	{
		if (this != &stSrc)
		{
			m_iTradeTimeID = stSrc.m_iTradeTimeID;
			m_aClientTradeTime.Copy(stSrc.m_aClientTradeTime);
		}

		return *this;
	}

	//
	int32				m_iTradeTimeID;
	arrClientTradeTime	m_aClientTradeTime;

}T_TradeTimeInfo;

typedef CArray<T_TradeTimeInfo, T_TradeTimeInfo&> arrClientTradeTimeInfo;

///////////////////////////////////////
// ��Ʒ���
///////////////////////////////////////

// ��Ʒ������Ϣ
class DATAINFO_DLL_EXPORT CMerchInfo
{
public:
	CMerchInfo()
	{
		m_iMarketId = -1;
		m_iSaveDec = 1;
		m_bRemoved = false;
	}

public:
	int32			m_iMarketId;		// �����г�����
	CString			m_StrMerchCode;		// ��Ʒ����
	CString			m_StrMerchCodeInBourse;		// ��Ʒ���������루����Ʒ����ʱʹ�õĴ��룬 Ŀǰ���ڻ����ԣ� ����Ŀǰ�����Ĵ��붼�Ƿ�����������ģ� �Ƚ�ͳһ�ĸ�ʽ�� �����ʶ�� ����ʵ�ʵĽ��������б��ڸô���ģ���
	CString			m_StrMerchEnName;	// ��ƷӢ����
	CString			m_StrMerchCnName;	// ��Ʒ������
	int32			m_iSaveDec;			// ����С����λ��
	int32			m_iTradeTimeID;		// ����ʱ��ID�����Ϊ��ʾʹ���г���ʱ��
	int32			m_iTradeTimeType;	// ����ʱ������

	// 
	bool32			m_bRemoved;			// ����Ʒ�Ƿ��Ѳ������ڸ��г���
};

///////////////////////////////////////////////////////////////////////////////////////////////////////

// ��Ʒ��չ��Ϣ
class DATAINFO_DLL_EXPORT CMerchExtendData
{
public:
	CMerchExtendData();
	~CMerchExtendData();
	
public:
	// zhangbo 0320 #�д�����
	//...
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////

// F10
class DATAINFO_DLL_EXPORT CMerchF10Node
{
public:
	CString			m_StrTitle;
	CString			m_StrContent;
};


/////////////////////////////////////////////////////////////////////////////////////////////////////////

class DATAINFO_DLL_EXPORT CMerchF10
{
public:
	CMerchF10();
	~CMerchF10();
	
public:
	void			ParseF10String(CString StrF10);

public:
	const CMerchF10& operator=(const CMerchF10& MerchF10Src);

public:
	CArray<CMerchF10Node, CMerchF10Node> m_F10Nodes;
};


/////////////////////////////////////////////////////////////////////////////////////////////////////////

class DATAINFO_DLL_EXPORT CSmartAttendMerch
{
public:
	CSmartAttendMerch();

public:
	const bool32 operator ==( const CSmartAttendMerch& SmartAttendMerch);
public:
	CMerch		*m_pMerch;
	UINT		m_iDataServiceTypes;		// ȡֵ E_DataServiceType�Ļ�ֵ
	bool32		m_bNeedNews;				// ...fangz0124 ��ʱ����,  �Ƿ���Ҫ��Ѷ֪ͨ
};
typedef CArray<CSmartAttendMerch, CSmartAttendMerch&> SmartAttendMerchArray;


//////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct T_AttendMarketData
{
	int32				m_iMarketId;		// ���ĵ��г�
	int					m_iEDSTypes;		// �����������ͼ��� E_DataServiceType
}T_AttendMarketData;
typedef CArray<T_AttendMarketData, const T_AttendMarketData &> AttendMarketDataArray;


/////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��ʱָ������
typedef struct T_MerchTrendIndex
{
public:
	T_MerchTrendIndex()
	{
		memset(this, 0, sizeof(T_MerchTrendIndex));
	}
	
	bool32  BeValid()
	{
		if ( m_lTime <= 0 )
		{
			return false;
		}

		if ( m_fVolumeRate <= 0 && m_fBuyVolumeSum <= 0 &&  m_fSellVolumeSum <= 0 )
		{
			return false;
		}

		//if ( m_fVolumeRate <= 0 && )
		//{
		//	return false;
		//}

		//if ( m_fBuyVolumeSum <= 0 )
		//{
		//	return false;
		//}

		//if ( m_fSellVolumeSum <= 0 )
		//{
		//	return false;
		//}

		return true;
	}

	long    m_lTime;				// ʱ��
	float	m_fVolumeRate;			// ����
	float	m_fBuyVolumeSum;		// �嵵����֮��
	float	m_fSellVolumeSum;		// �嵵����֮��
	
} T_MerchTrendIndex;

/////////////////////////////////////////////////////////////////////////////////////////////////////////

// ���Ͼ�������
typedef struct T_MerchAuction
{
public:
	T_MerchAuction()
	{
		memset(this, 0, sizeof(T_MerchAuction));
	}
	
	long	m_lTime;				// ʱ��
	float	m_fBuyPrice;			// ���
	float	m_fBuyVolume;			// ����
	char	m_cFlag;				// ����
	
} T_MerchAuction;


/////////////////////////////////////////////////////////////////////////////////////////////////////////

// ��������������
typedef struct T_MinuteBS
{
public:
	T_MinuteBS()
	{
		memset(this, 0, sizeof(T_MinuteBS));
	}
	
	bool32 BeValid()
	{
		if ( m_lTime <= 0 )
		{
			return false;
		}

		if ( m_fBuyVolume < 0 )
		{
			return false;
		}

		if ( m_fSellVolume < 0 )
		{
			return false;
		}

		return true;
	}

	long    m_lTime;				// ʱ��	
	float	m_fBuyVolume;			// ����
	float	m_fSellVolume;			// ����
	
} T_MinuteBS;


/////////////////////////////////////////////////////////////////////////////////////////////////////////
// ������֧�ֵĸ����ļ�����
enum E_PublicFileType
{
	EPFTWeight = 0,		// ��Ȩ
	EPFTF10,			// F10����
	EPFTExtend,			// ��չ����(�ݲ�֧��)
	
	//
	EPFTCount
};

// ��Ȩ����
struct LONDATE { public: DWORD Minute:6; DWORD Hour:5; DWORD Day:5; DWORD Month:4; DWORD Year:12; };

#pragma pack(1) 
struct T_WeightFileFormat
{
	LONDATE Date; 
	DWORD A,B,C,D,E;		// �͹���,�����,��ɼ�,����,������ 
	DWORD Base,FlowBase;	// �ܹɱ�,��ͨ�ɱ� 
	DWORD Reserved;
};
#pragma pack()


/////////////////////////////////////////////////////////////////////////////////////////////////////////

class DATAINFO_DLL_EXPORT CWeightData
{
public:
	CWeightData()
	{
		B01 = B02 = B03 = B04 = B05 = 0.;
		Base = FlowBase = 0;
	}

public:
	void Init(const T_WeightFileFormat &WeightFileFormat)
	{
		m_TimeCurrent = CGmtTime(WeightFileFormat.Date.Year, WeightFileFormat.Date.Month, WeightFileFormat.Date.Day, WeightFileFormat.Date.Hour, WeightFileFormat.Date.Minute, 0);

		B01 = (float)WeightFileFormat.D;
		B02 = (float)WeightFileFormat.C;
		B03 = (float)WeightFileFormat.A;
		B04 = (float)WeightFileFormat.B;
		B05 = (float)WeightFileFormat.E;

		Base = WeightFileFormat.Base;
		FlowBase = WeightFileFormat.FlowBase;
	}

	void WeightPrice(INOUT float &fPrice, bool32 bFront) const 
	{
		if (bFront)	// ǰ��Ȩ�� ��Ȩ��=(ԭ��-����+��ɽ��)/(ԭ�ɱ�+�͹�+���)
		{
			if (10.0 + B03 + B04 != 0.)
			{
				// ͨ�����㷨
				//fPrice = (10.0 * fPrice - B01 + B04 * B02) / (10.0 + B03 + B04);	

				// ���㷨
				fPrice *= 1000;
				fPrice = (float)((fPrice * 10.0 + B04 * 0.0001 * B02 - B01) / (10.0 + B03 * 0.0001 + B04 * 0.0001 + B05 * 0.0001));
				fPrice /= 1000;
			}
		}
		else		// ��Ȩ�� 
		{
			// ͨ�����㷨
			//fPrice = (fPrice * (10.0 + B03 + B04) + B01 - B04 * B02) / 10.0;

			// ���㷨
			fPrice *= 1000;
			fPrice = (float)(((10.0 + B03 * 0.0001 + B04 * 0.0001 + B05 * 0.0001) * fPrice - B04 * 0.0001 * B02 + B01) * 0.1);
			fPrice /= 1000;
		}
	}

	void WeightVolumeAndAmount(INOUT float &fVolume, INOUT float &fAmount, bool32 bFront) const
	{
		if (bFront)	// ǰ��Ȩ�� 
		{
			//fAmount = fAmount + B02 * B04 / 10 * fVolume - fVolume * B01 / 10;
			//fVolume = fVolume + fVolume * (B03 + B04) / 100000.0;
			// �ɽ���䣬�ɽ�����۸�ı仯�ɷ���
			if ( 0.0f != fVolume )
			{
				float fPriceOld = fAmount/fVolume;
				WeightPrice(fPriceOld, bFront);
				fVolume = fAmount/fPriceOld;
			}
		}
		else		// ��Ȩ�� 
		{
			if (10 + B03 + B04 != 0)
			{
				//fVolume = fVolume * 100000.0 / (100000 + B03 + B04);
				//fAmount = fAmount - B02 * B04 / 10 * fVolume + fVolume * B01 / 10;
				// �ɽ���䣬�ɽ�����۸�ı仯�ɷ���
				if ( 0.0f != fVolume )
				{
					float fPriceOld = fAmount/fVolume;
					WeightPrice(fPriceOld, bFront);
					fVolume = fAmount/fPriceOld;
				}
			}		
		}
	}

public:
	CGmtTime		m_TimeCurrent;	// ʱ�䣨�գ�
	
	float			B03;			// �͹���
	float			B04;			// �����
	float			B02;			// ��ɼ�
	float			B01;			// ����
	float			B05;			// ������
	
	DWORD			Base;			// �ܹɱ�
	DWORD			FlowBase;		// ��ͨ�ɱ�
};


/////////////////////////////////////////////////////////////////////////////////////////////////////////

class DATAINFO_DLL_EXPORT CFinanceData
{
public:
	CFinanceData()
	{		
		m_fAllCapical       = 0.;		
		m_fCircAsset		= 0.;		
		m_fAllAsset         = 0.;		
		m_fFlowDebt         = 0.;		
		m_fPerFund          = 0.;			
		
		m_fBusinessProfit   = 0.;	
		m_fPerNoDistribute  = 0.;	
		m_fPerIncomeYear    = 0.;	
		m_fPerPureAsset     = 0.;	
		m_fChPerPureAsset   = 0.;	
		
		m_fDorRightRate     = 0.;	
		m_iIncomeSeason		= 0;
	}

public:
	float	m_fAllCapical;			// �ܹɱ�
	float	m_fCircAsset;			// ��ͨ�ɱ�
	float	m_fAllAsset;			// ���ʲ�
	float	m_fFlowDebt;			// ������ծ
	float	m_fPerFund;				// ÿ�ɹ�����
	
	float	m_fBusinessProfit;		// Ӫҵ����
	float	m_fPerNoDistribute;		// ÿ��δ����
	float	m_fPerIncomeYear;		// ÿ������(��)
	float	m_fPerPureAsset;		// ÿ�ɾ��ʲ�
	float	m_fChPerPureAsset;		// ����ÿ�ɾ��ʲ�
	
	float	m_fDorRightRate;		// �ɶ�Ȩ���
	int32	m_iIncomeSeason;		// ���漾��

};

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// �г�ʱ��
class DATAINFO_DLL_EXPORT CMarketIOCTimeInfo
{
public:
	CMarketIOCTimeInfo(){}
	CMarketIOCTimeInfo(const CMarketIOCTimeInfo &Src)
	{
		m_TimeInit	= Src.m_TimeInit;
		m_TimeOpen	= Src.m_TimeOpen;
		m_TimeClose = Src.m_TimeClose;
		m_TimeEnd	= Src.m_TimeEnd;
		
		m_aOCTimes.Copy(Src.m_aOCTimes);
	}
	const CMarketIOCTimeInfo& operator=(const CMarketIOCTimeInfo &Src)
	{
		m_TimeInit	= Src.m_TimeInit;
		m_TimeOpen	= Src.m_TimeOpen;
		m_TimeClose = Src.m_TimeClose;
		m_TimeEnd	= Src.m_TimeEnd;
		
		m_aOCTimes.Copy(Src.m_aOCTimes);
		
		return *this;
	}
	
	// ���㵱ǰ��Ʒһ�������յķ���������
	int32 GetMaxTrendUnitCount() const
	{
		int32 iCount = 0;
		for (int32 i = 0; i < m_aOCTimes.GetSize(); i += 2)
		{
			iCount += (int32)((m_aOCTimes[i + 1].GetTime() - m_aOCTimes[i].GetTime()) / 60 + 1);
		}
		
		return iCount;
	}
	
public:
	CMsTime		m_TimeInit;		// һ�쿪ʼ
	CMsTime		m_TimeOpen;		// ����
	CMsTime		m_TimeClose;	// ����			// ���ڿ��콻�ף� �����հ�Closeʱ��������Ϊ׼�� ���������㣬 ����һ�Ľ���ʱ��Ϊ��������17:00 ~ ����һ17:00
	CMsTime		m_TimeEnd;		// һ�����
	
	CArray<CGmtTime, const CGmtTime&>	m_aOCTimes;		// ���� + ��Ϣ + ���� + ��Ϣ + ..., ����˫����
};

// ����������Ϣ
#define PRICE_ARRAY_SIZE	20			// 20��

/////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct T_PriceVolume
{
public:
	T_PriceVolume()
	{
		m_fPrice	= 0.;
		m_fVolume	= 0.;
	}

public:
	float			m_fPrice;
	float			m_fVolume;
}T_PriceVolume;

/////////////////////////////////////////////////////////////////////////////////////////////////////////

class DATAINFO_DLL_EXPORT CRealtimePrice
{
public:
	enum E_UpdateKLineType
	{
		EUKTNone = 0,			// ������K�ߣ� �����ñ����ݲ���һ�ʽ������ݣ� �ܿ�����5�����鷢���˱仯
		EUKTUpdate,				// ������������(�գ��£� �֣� 5�֣� 60��)��K��
		EUKTUpdateOnlyDayKLine,	// ����������

		// 
		EUKTCount
	};

public: 
	CRealtimePrice();
	~CRealtimePrice();

public:
	bool32			operator==(const CRealtimePrice &RealtimePrice) const;
	// CRealtimePrice& CRealtimePrice::operator= (const CRealtimePrice& RealtimePrice);

public:
	void			ReviseVolume(int32 iVolScale);
	void			InitMerchPrice();	// ���Ͼ���ʱ�䣬 ��ʼ���г�

public:
	E_UpdateKLineType	m_eUpdateKLineType;

public:
	int32			m_iMarketId;		// �г����
	CString			m_StrMerchCode;		// ��Ʒ����

	// 	
	CMsTime			m_TimeCurrent;

	// 
	float			m_fPricePrevClose;	// ǰ�ռ�
	float			m_fPricePrevAvg;	// ǰ����

	// 
	float			m_fPriceOpen;		// ���̼�
	float			m_fPriceHigh;		// ��߼�
	float			m_fPriceLow;		// ��ͼ�
	float			m_fPriceNew;		// ���¼�
	float			m_fPriceAvg;		// �����

	// 
	float			m_fHoldPrev;		// ǰ�ֲ�

	float			m_fVolumeCur;		// ��ǰ�ɽ�������Ʊ�еġ����֡���
	float			m_fVolumeTotal;		// �ܳɽ��� ����Ʊ�еġ����֡���
	float			m_fAmountTotal;		// �ɽ����
	float			m_fHoldCur;			// �ڻ��е����֣�������Ʒ�������� 
	float			m_fHoldTotal;		// �ڻ��гֲ�����������Ʒ��������
	
	// 
	float			m_fVolumeRate;		// ����
	float			m_fBuyVolume;		// ����, ����۳ɽ�
	float			m_fSellVolume;		// ����, �����ҳɽ�

	// ���塢����
	T_PriceVolume	m_astBuyPrices[PRICE_ARRAY_SIZE];	// ����б� ��һ���ߣ�-> ���壨�ͣ�
	T_PriceVolume	m_astSellPrices[PRICE_ARRAY_SIZE];	// �����б� ��һ���ͣ�-> ���壨�ߣ�
	
	//
	float			m_fPeRate;				// ��ӯ��
	float			m_fTradeRate;			// ������	
	float			m_fRiseRate;			// ����
	float			m_fCapticalFlow;		// �ʽ�����

	//
	float			m_fSellAmount;			// ���̳ɽ���	
	UINT			m_uiSeason;				// ����
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////

enum E_TickType
{
	ETTUnknown  = 0,	// ������
	ETTBuy		= 1,	// ���
	ETTSell		= 2,	// ����
	ETTTrade	= 4,	// �ɽ���

	// 
	ETTCount		
};

enum E_RealtimePushType
{
	ERPTUnknown			= 0,	// 
	ERPTPushPrice		= 1,	// ����������
	ERPTPushTick		= 2,	// ����Tick
	ERPTPushLevel2		= 4,	// ����Level2
		
	// 
	ERPTCount		
};

// �ɽ���ϸ����
class DATAINFO_DLL_EXPORT CTick
{
public:
	enum E_TradeKind
	{
		ETKUnKnown = 0,		// ����
		ETKBuy,				// ����۳ɽ�,����
		ETKSell,			// �����۳ɽ�,����
		
		//
		ETKCount		
	};

public:
	CTick();

public:
	bool32			MaybeFault() const;
	bool32			operator==(const CTick &Tick) const;

	void			ReviseVolume(int32 iVolScale);

public:
	E_TickType		m_eTickType;		// ����

public:
	CMsTime			m_TimeCurrent;
	
	float			m_fPrice;			// �۸�
	float			m_fVolume;			// ����
	float			m_fAmount;			// �ñʳɽ����
	float			m_fHold;			// �ֲܳ���
	E_TradeKind		m_eTradeKind;		// ��������Ϣ
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////

class DATAINFO_DLL_EXPORT CPrevPriceInfo
{
public:
	CPrevPriceInfo() 
	{
		m_uiDay = 0;
		m_fPrevClosePrice = 0.;
		m_fPrevAvgPrice = 0.;
	}
	
public:
	uint32			m_uiDay;				// ����¼����������
	float			m_fPrevClosePrice;		// ���ռ�
	float			m_fPrevAvgPrice;		// �����
};

class DATAINFO_DLL_EXPORT CMerchTimeSales
{
public:
	CMerchTimeSales();
	~CMerchTimeSales();

public:
	
	static int32	QuickFindTickByTime(const CTick *pTicks, int32 iTickCount, const CMsTime &TimeRefer, bool32 bNearest);
	static int32	QuickFindTickByTime(const CArray<CTick, CTick> &aTicks, const CMsTime &TimeRefer, bool32 bNearest);

	static int32	QuickFindTickWithBigOrEqualReferTime(const CTick *pTicks, int32 iTickCount, const CMsTime &TimeRefer);
	static int32	QuickFindTickWithBigOrEqualReferTime(const CArray<CTick, CTick> &aTicks, const CMsTime &TimeRefer);

	static int32	QuickFindTickWithSmallOrEqualReferTime(const CTick *pTicks, int32 iTickCount, const CMsTime &TimeRefer);
	static int32	QuickFindTickWithSmallOrEqualReferTime(const CArray<CTick, CTick> &aTicks, const CMsTime &TimeRefer);

public:
	// ����ֱʳɽ���ʷ����
	bool32			ProcessTimesSaleData(const CMerchTimeSales	&MerchTimeSales);

	// ����ʵʱ�ֱ�����
	bool32			ProcessRealtimeTick(const CTick &RealtimeTick);

public:
	int32			m_iMarketId;		// �г����
	CString			m_StrMerchCode;		// ��Ʒ����
	
	CArray<CTick, CTick> m_Ticks;		// �ֱ��б�

	uint32			m_uiTailTime;		// ����ʱ��
};


/////////////////////////////////////////////////////////////////////////////////////////////////////////

// level2
class DATAINFO_DLL_EXPORT CLevel2Detail
{
public:
	CLevel2Detail()
	{
		m_bPos		= false;
		m_bBuy		= false;
		
		m_fPrice	= 0.;
		m_fVolume	= 0.;
	}

public:
	// ������ǰ�ṹ�ǲ��������������µ�������
	bool32			m_bPos;

	// ����Ǿ�����
	CString			m_StrBrokerCode;	// ��������ϯλ����
	CString			m_StrBrokerName;	// ��������ϯλ����
	bool32			m_bBuy;				// ��/����־
	
	// ������Ǿ����ˣ���λ��
	CString			m_StrPos;			// ��λ���ƣ�����ǵ�λ��

	// ���������ֶ�Ŀǰû������
	float			m_fPrice;			// �� 
	float			m_fVolume;			// ��������
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////

class DATAINFO_DLL_EXPORT CLevel2Node
{
public:
	enum E_Level2Type { ELTBuy = 0, ELTSell = 1 };	// level2��/��

public:
	CLevel2Node(); 
	CLevel2Node(const CLevel2Node& Level2NodeSrc);
	~CLevel2Node();

public:
	const CLevel2Node& operator=(const CLevel2Node& Level2NodeSrc);
	
public:
	float			m_fPrice;		// ��������
	E_Level2Type	m_eLevel2Type;	// ��/��
	float			m_fVolume;		// �ܳɽ�������������ϯλ�ڸü�λ�����������ܺͣ�
	CArray<CLevel2Detail, CLevel2Detail> m_Level2Details;	// ���������̼�λ�ϸ�������ϯλ����Ϣ
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////
class DATAINFO_DLL_EXPORT CRealtimeLevel2
{
public:
	CRealtimeLevel2();
	~CRealtimeLevel2();

public:
	const CRealtimeLevel2& operator=(const CRealtimeLevel2& RealtimeLevel2Src);

public:
	CGmtTime		m_TimeCurrent;		// ��ʱ��Ϊ��Ʒ����ʱ����ʱ�䣬 ��������ֶθ�ֵ�� ������Ϊ0ֵ�� ϵͳ����Ը��ֶεĴ�������ù����� �������ȷ�ȵ�����

	int32			m_iMarketId;		// �г����
	CString			m_StrMerchCode;		// ��Ʒ����
	
	float			m_fPriceAveBuy;		// �������
	float			m_fPriceAveSell;	// ��������
	
	float			m_fVolumeBuyTotal;	// �����ܺ�
	float			m_fVolumeSellTotal;	// �����ܺ�

	CArray<CLevel2Node, CLevel2Node> m_Level2BuyNodes;	// ����б� �Ӹ߼۵��ͼ�
	CArray<CLevel2Node, CLevel2Node> m_Level2SellNodes;	// �����б� �ӵͼ۵��߼�


	CArray<CLevel2Detail, CLevel2Detail> m_xxxBuyLevel2List;	// ���������̼�λ�ϸ�������ϯλ����Ϣ
	CArray<CLevel2Detail, CLevel2Detail> m_yyySellLevel2List;	// ���������̼�λ�ϸ�������ϯλ����Ϣ
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// K��
class DATAINFO_DLL_EXPORT CKLine
{
public:
	CKLine()
	{
		m_fPriceOpen		= 0.;
		m_fPriceClose		= 0.;
		m_fPriceAvg			= 0.;
		m_fPriceHigh		= 0.;
		m_fPriceLow			= 0.;

		m_fVolume			= 0.;
		m_fAmount			= 0.;
		m_fHold				= 0.;

		m_usRiseMerchCount	= 0;
		m_usFallMerchCount	= 0;
	}

public:
	bool32			MaybeFault() const;
	bool32			operator==(const CKLine &KLine) const;
	CKLine&			operator+=(const CKLine &KLine);
	
	void			ReviseVolume(int32 iVolScale);

public:
	CGmtTime		m_TimeCurrent;		// ��ʱ����Բ�ͬ������K�ߣ� Ӧ�ý�������ֶθ�ֵ�� ������Ϊ0ֵ�� ϵͳ����Ը��ֶεĴ�������ù����� ��������ߣ� ʱ���֡��������Ϊ0

	// 
	float			m_fPriceOpen;		// ��
	float			m_fPriceClose;		// ��
	float			m_fPriceAvg;		// ���㣨����
	float			m_fPriceHigh;		// ��
	float			m_fPriceLow;		// ��

	// 
	float			m_fVolume;			// ��
	float			m_fAmount;			// ��
	float			m_fHold;			// ��	
	
	// ���������ֶν��Դ���������
	uint16			m_usRiseMerchCount;		// �����г��ڸ������ڵ����Ǽ���
	uint16			m_usFallMerchCount;		// �����г��ڸ������ڵ��µ�����
};


/////////////////////////////////////////////////////////////////////////////////////////////////////////
enum E_KLineTypeBase	// ����K������, ������K�����ͻ����ھ������»���K����϶���
{
	EKTBMinute = 0,		// ������
	EKTBDay,			// ����
	EKTB5Min,			// 5������
	EKTBHour,			// Сʱ��
	EKTBMonth,			// ����
	
	//
	EKTBCount			// ������������
};


class DATAINFO_DLL_EXPORT CMerchKLineNode
{
public:
	CMerchKLineNode();
	virtual ~CMerchKLineNode();

	CMerchKLineNode& CMerchKLineNode::operator= (const CMerchKLineNode& stSrc)
	{
		if ( this == &stSrc )
		{
			return *this;
		}

		//
		m_iMarketId		= stSrc.m_iMarketId;			// �г����
		m_StrMerchCode	= stSrc.m_StrMerchCode;			// ��Ʒ����		
		m_eKLineTypeBase= stSrc.m_eKLineTypeBase;		// K������
		m_KLines.Copy(stSrc.m_KLines);					// K������		
		m_uiTailTime = stSrc.m_uiTailTime;				// ����ʱ��

		return *this;
	}

public:
	// ���ֲ��ҿ��ٶ�λ
	static int32	QuickFindKLineByTime(const CArray<CKLine, CKLine> &aKLines, const CGmtTime &TimeRefer, bool32 bNearest = false);
	static int32	QuickFindKLineWithBigOrEqualReferTime(const CArray<CKLine, CKLine> &aKLines, const CGmtTime &TimeRefer);
	static int32	QuickFindKLineWithSmallOrEqualReferTime(const CArray<CKLine, CKLine> &aKLines, const CGmtTime &TimeRefer);
	
	// K�ߺϲ�
	static bool32	GetKLineTimeByMinute(const IN CMerch* pMerch, IN UINT uiTimeCurrent, IN int32 iMinutePeriod, OUT UINT &uiTimeStart, OUT UINT &uiTimeEnd);	
	static bool32	CombinMinuteN(const IN CMerch* pMerch, IN int32 iCombinPeriod, IN const CArray<CKLine, CKLine> &aKLinesSrc, OUT CArray<CKLine, CKLine> &aKLinesDst);
	static bool32	CombinDayN(IN const CArray<CKLine, CKLine> &aKLinesSrc, IN int32 iCombinPeriod, OUT CArray<CKLine, CKLine> &aKLinesDst);
	static bool32	CombinMonthN(IN const CArray<CKLine, CKLine> &aKLinesSrc, IN int32 iCombinPeriod, OUT CArray<CKLine, CKLine> &aKLinesDst);
	static bool32	CombinWeek(IN const CArray<CKLine, CKLine> &aKLinesSrc, OUT CArray<CKLine, CKLine> &aKLinesDst);

	// ��Ȩ
	// bFront			-- ǰ��Ȩ
	// bWeightVolume	-- ��Ȩ�ɽ���
	// uiTimeSpecify	-- ���㸴Ȩʱ�䣬 Ĭ��0xffffffff��ʾ���Ը�ʱ��
	static bool32	WeightKLine(IN const CArray<CKLine, CKLine> &aKLinesSrc, IN const CArray<CWeightData, CWeightData&> &aWeightDatas, bool32 bFront, OUT CArray<CKLine, CKLine> &aKLinesDst, IN bool32 bWeightVolume = false, IN uint32 uiTimeSpecify = 0xffffffff);
	static void		DoWeight(INOUT CKLine &KLine, const CWeightData *pWeightDatas, int32 iWeightIndex, int32 iWeightCount, bool32 bFront, bool32 bWeightVolume);	// ���㸴Ȩ

	//
	static bool32	CheckKLineList(const CArray<CKLine, CKLine> &aKLines);

private:	
	//
	static bool32	GetKLineTimeByMinute(IN int32 iTimeInitPerDay, IN const CArray<T_OpenCloseTime, T_OpenCloseTime> &aOpenCloseTimes, IN UINT uiTimeCurrent, IN int32 iMinutePeriod, OUT UINT &uiTimeStart, OUT UINT &uiTimeEnd, bool32 bNeedAdjust);
	static bool32	CombinMinuteN(IN int32 iTimeInitPerDay, IN const CArray<T_OpenCloseTime, T_OpenCloseTime> &aOpenCloseTimes, IN int32 iCombinPeriod, IN const CArray<CKLine, CKLine> &aKLinesSrc, OUT CArray<CKLine, CKLine> &aKLinesDst);

public:
	// ����һ��K������
	bool32			ProcessKLindData(const CMerchKLineNode &MerchKLineNode);

	// ����һ��ʵʱ����
	// bool32			ProcessRealtimePrice(IN int32 iTimeInitPerDay, const CArray<T_OpenCloseTime, T_OpenCloseTime> &aOpenCloseTimes, const CRealtimePrice &RealtimePrice);
	
	bool32			ProcessRealtimePrice(IN CMerch* pMerch, const CRealtimePrice &RealtimePrice);

private:
	// ����һ��K������
	bool32			AddKLine(IN const CKLine &KLine, int32 iPosStart, OUT int32 &iPosAdd);
	
	
public:
	int32			m_iMarketId;			// �г����
	CString			m_StrMerchCode;			// ��Ʒ����

	E_KLineTypeBase	m_eKLineTypeBase;		// K������
	CArray<CKLine, CKLine> m_KLines;		// K������
	
	uint32			m_uiTailTime;			// ����ʱ��
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��Ʒ������Ϣ 
class DATAINFO_DLL_EXPORT CMerch
{
public:
	CMerch(CMarket &Market);
	~CMerch();

public:
	static bool32	ReadWeightDatasFromBuffer(const char *pcData, int32 iDataLen, OUT CArray<CWeightData, CWeightData&> &aWeightDatas);
	static bool32	ReadFinanceDataFromBuffer(const char *pcData, int32 iDataLen, OUT CFinanceData& FinanceData);

public:
	void			FreeAllHistoryData();
	void			FreeAllRealtimeData();
 
	bool32			DoesHaveHistoryData();

	bool32			DoesNeedPushPrice(IN const CGmtTime &TimeNow);
	bool32			DoesNeedPushTick(IN const CGmtTime &TimeNow);
	bool32			DoesNeedPushLevel2(IN const CGmtTime &TimeNow);

	bool32			DoesNeedReqWeightData(IN const CGmtTime &TimeNow);
	bool32			DoesNeedReqFinanceData(IN const CGmtTime &TimeNow);

public:
	// ����ָ�����ͣ�K�߻������ͣ���K����ʷ����
	bool32			FindMerchKLineNode(E_KLineTypeBase eKLinetypeBase, OUT int32 &iPosFound, CMerchKLineNode *&pMerchKLineNodeFound) const;

	// ����ָ��ĳ������ռۣ� Ϊ����ͼ��ʾ��
	bool32			GetSpecialDayPrice(const CGmtTime &TimeSpecialDay, float &fPrevClosePrice, float &fPrevAvgPrice) const;

	// ����ָ��ĳ��ĳֲ�
	bool32			GetSpecialDayHold(const CGmtTime &TimeSpecialDay, float &fPrevHold) const;

	// ��ȡ���ʱ��
	bool32			GetOCTime(IN int32 uiTimeCurrent, OUT int32& iTimeInit, OUT CArray<T_OpenCloseTime, T_OpenCloseTime>& aOCTimes);

public:
	// ���Ͼ������ݸ���
	void			OnMerchAucitonsUpdate(const CArray<T_MerchAuction, T_MerchAuction&>& aAuctions);

	// ��ʱ�������ݸ���
	bool32			OnMerchTrendIndexUpdate(const CArray<T_MerchTrendIndex, T_MerchTrendIndex&>& aMerchTrendIndex);

	// �������������ݸ���
	bool32			OnMerchMinuteBSUpdate(const CArray<T_MinuteBS, T_MinuteBS&>& aMerchMinuteBS);

	CString			GetMerchVarName();
public:
	CMerchInfo		m_MerchInfo;			// ��Ʒ������Ϣ
	
	// �������ݷ�ʵʱ���ݣ� ����һ�飬 һ���ڲ���Ҫ���£� ��������
	CMerchExtendData	*m_pMerchExtendData;// ��Ʒ��չ����
	CMerchF10		*m_pMerchF10;			// ��ƷF10��Ϣ
	
	// ��������Ϊʵʱ���͵�����
	CRealtimePrice	*m_pRealtimePrice;		// ��Ʒ��ʱ5������
	CTick			*m_pRealtimeTick;		// ��Ʒ��ʱ�ֱ���Ϣ
	CRealtimeLevel2	*m_pRealtimeLevel2;		// ��Ʒ��ʱlevel2����

	// ��������Ϊ��Ʒ����ʷ����
	CArray<CMerchKLineNode*, CMerchKLineNode*> m_MerchKLineNodesPtr;// ��Ʒ�ļ�����������K������, �����ÿһ��K�����ݶ��Ǵӵ�ǰʱ��������ݣ� ��������м���һ������
	CMerchTimeSales	*m_pMerchTimeSales;		// ��Ʒ�ɽ���ϸ
	
	// ��Ȩ����
	CArray<CWeightData, CWeightData&> m_aWeightDatas;
	uint32			m_uiWeightDataCRC;
	CGmtTime		m_TimeLastUpdateWeightDatas;
	bool32			m_bHaveReadLocalWeightFile;		// �Ƿ�ӱ��ض�ȡ����Ȩ����

	// �������� fangz 0818 ��ʱû�б��汾�ز�������. ���Ǵӷ����������
	CGmtTime		m_TimeLastUpdateFinanceData;	// ���һ���յ��������ݣ�����ʱ���һ�β��������
	CFinanceData*	m_pFinanceData;

	// ��Ϣ����
	mapLandMine		m_mapLandMine;

	// ���Ͼ�������
	CArray<T_MerchAuction, T_MerchAuction&>	m_aAuctionDatas;

	// ��ʱ����ָ������
	CArray<T_MerchTrendIndex, T_MerchTrendIndex&> m_aMerchTrendIndexs; 

	// ��������������
	CArray<T_MinuteBS, T_MinuteBS&>	m_aMinuteBS;

	// ѡ��ʱ��
	uint32 m_uiSelStockTime;
	// ��ѡ�۸�
	float  m_fSelPrice;

public:
	CMarket			&m_Market;				// ��Ʒ�����г�ָ�룬 ���ٶ�λ�г�
	CString			m_StrMerchFakeName;
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// ����ϯλ��Ϣ
///////////////////////////////////////////////
class DATAINFO_DLL_EXPORT CBroker
{
public:
	CBroker(CBreed &Breed);
	~CBroker();

public:
	const CBroker& operator=(const CMmiBroker& MmiBroker);

public:
	CString			m_StrBrokerCode;	// ����ϯλ����
	CString			m_StrBrokerCnName;	// ����ϯλ����
	CString			m_StrBrokerEnName;

	bool32			m_bRemoved;

public:
	CBreed			&m_Breed;				// ��Ʒ��������Ʒ��ָ�룬 ���ٶ�λ����Ʒ��
};


// �г���Ϣ  
class DATAINFO_DLL_EXPORT CMarketInfo 
{
public:
	CMarketInfo();
	~CMarketInfo();

public:
	static bool32	GetDayTime(IN int32 iTimeInitPerDay, IN const CArray<T_OpenCloseTime, T_OpenCloseTime> &aOpenCloseTimes, IN UINT uiTimeCurrent, OUT UINT &uiTimeStart, OUT UINT &uiTimeEnd, OUT UINT &uiTimeOpen, OUT UINT &uiTimeClose);

public:
	const CMarketInfo& operator=(const CMarketInfo& MarketInfoSrc);
	void GetTimeInfo(int32 &iTimeInitPerDay, int32 &iTimeOpenPerDay, int32 &iTimeClosePerDay);	// ȡ�г��ĳ�ʼ��,��,����ʱ��

public:
	int32			m_iMarketId;			// �г����
	int32			m_iShowId;				// ��ʾ��
	int32			m_iTimeZone;			// ���г�����ʱ��

	//
	CString			m_StrBourseCode;		// �����ڽ��������г�����
	CString			m_StrEnName;			// Ӣ������
	CString			m_StrCnName;			// ��������
	
	// 
	int32			m_iTimeInitializePerDay;// ÿ���ʼ��ʱ�䡣��ʽ���Է���Ϊ��λ�� �����00:00�ֵ�ֵ
											// �ͻ���ÿ�춼��Ҫ��ϵͳ����Ʒ�б�����ʼ���� 
											// ��ʱ������ÿ�γ�ʼ����ʱ�䣨��������ʱ�䣩�� ���ڹ��ڹ��У� ��ʱ�伴�����Ͼ��ۡ�ʱ��
	// 
	CArray<T_OpenCloseTime, T_OpenCloseTime>	m_OpenCloseTimes;	// ÿ�켸������ʱ��Σ���������ʱ�䣩

	// ������Ľ����պ�ǰ�Ľ�����(���������,Ȼ��������10��������)
	CArray<uint32, uint32>	m_aRecentActiveDay;	

	//
	E_ReportType	m_eMarketReportType;	// �г��ı�������

	//
	int32			m_iVolScale;			// �ɽ�����ʾʱ, ��Ҫ���ı���
};

// �г�����
class DATAINFO_DLL_EXPORT CMarketSnapshotInfo
{
public:
	CMarketSnapshotInfo() { m_iMarketId = -1; m_iRiseCount = 0; m_iFallCount = 0; }

public:
	bool32 operator==(const CMarketSnapshotInfo &MarketSnapshotInfo) const
	{
		if (m_iMarketId != MarketSnapshotInfo.m_iMarketId)
			return false;

		if (m_iRiseCount != MarketSnapshotInfo.m_iRiseCount)
			return false;

		if (m_iFallCount != MarketSnapshotInfo.m_iFallCount)
			return false;

		return true;
	}

public:
	int32			m_iMarketId;

	int32			m_iRiseCount;		// ���Ǽ���
	int32			m_iFallCount;		// �µ�����
};

// �г���ʼ��״̬
class DATAINFO_DLL_EXPORT CMarketReqState
{
public:
	CMarketReqState() { m_bRequesting = false; m_iCommunicationId = -1; }
	~CMarketReqState() { NULL; }

public:
	bool32			m_bRequesting;
	int32			m_iCommunicationId;
};

// ������Ʒ�Ľ���ʱ��
class DATAINFO_DLL_EXPORT CMerchClientTradeTimeInfo
{
public:
	CMerchClientTradeTimeInfo() {m_aClientTradeTimeInfo.RemoveAll();}
	~CMerchClientTradeTimeInfo(){}
public:
	static arrClientTradeTimeInfo	m_aClientTradeTimeInfo;	// ������Ʒ�Ľ���ʱ��
};


// �г��ṹ
class DATAINFO_DLL_EXPORT CMarket
{
public:
	CMarket(CBreed &m_Breed);
	~CMarket();
	
public:
	void			SetInitialized(bool32 bInitialized) { m_bInitialized = bInitialized; };

	// ���г��Ƿ��ʼ����
	bool32			IsInitialized() { return m_bInitialized; };
	
	// �Ƿ���Ҫ���³�ʼ��
	bool32			DoesNeedInitialize(IN const CGmtTime &TimeNow);
	
	// ����ָ���������Ʒ
	bool32			FindMerch(IN const CString &StrMerchCode, OUT int32 &iPosFound, OUT CMerch *&pMerchFound);

	// ����һ����Ʒ�����ж��Ƿ����ظ���
	bool32			AddMerch(CMerchInfo &MerchInfo);

	// ����ָ��ʱ���Ƿ���
	bool32			IsTradingDay(const CGmtTime &Time);
	
	// ��ȡĳһ����г�ʱ�������Ϣ(���ж��Ƿ��нڼ�����Ӱ�쵽ָ���첻���̵����)
	bool32			GetSpecialTradingDayTime(const CGmtTime &Time, CMarketIOCTimeInfo &TradingDayTime, const CMerchInfo& pMerchInfo);

	// ��ȡ���ǰ��3��������
	bool32			GetRecent3TradingDay(const CGmtTime &Time, bool32 &bValidPrevTradingDay, CMarketIOCTimeInfo &PrevTradingDayTime, bool32 &bValidCurTradingDay, CMarketIOCTimeInfo &CurTradingDayTime, bool32 &bValidNextTradingDay, CMarketIOCTimeInfo &NextTradingDayTime, const CMerchInfo& MerchInfo);

	// ��ȡ��ǰ��Ҫ��ʾ�Ľ�������Ϣ
	bool32			GetRecentTradingDay(const CGmtTime &Time, CMarketIOCTimeInfo &RecentTradingDayTime, const CMerchInfo& MerchInfo);

public:
	CMarketInfo		m_MarketInfo;			// �����г���Ϣ
	CMarketSnapshotInfo	m_MarketSnapshotInfo;	// �г��Ŀ�����Ϣ

	// 
	CGmtTime		m_TimeLastInitialize;	// ���һ�γ�ʼ���ɹ���ʱ�䣨���г�����ʱ����ʱ�䣩

	// 
	CArray<CMerch*, CMerch*> m_MerchsPtr;	// ���г���������Ʒ			// zhangbo 0326 #���Ż����ô�����������Ǿ�������ģ� ���й��ڸ����ݵ���ز������Զ����ö��ֲ��ҵ��㷨���ӿ���ҹ���
	std::map<CString, int32> m_aMapMerchs;	// ������ٲ�����Ʒ
	
	// �г���Ϣ����Ʒ�б�����״̬
	CMarketReqState	m_MarketInfoReqState;
	CMarketReqState	m_MerchListReqState;
	
public:
	CBreed			&m_Breed;				// ��������Ʒ����Ϣ

private: 
	bool32			m_bInitialized;			// �Ƿ��ʼ���������ٳ�ʼ����һ�Σ� �����ǳ���һ��������ǰ��ʼ����
};

///////////////////////////////////////////////
// ����Ʒ�֣��磺��½��Ʊ����۹�Ʊ�����
///////////////////////////////////////////////
class DATAINFO_DLL_EXPORT CBreed
{
public:
	CBreed();
	~CBreed();

public:
	// ����ָ��������г�
	bool32			FindMarket(int iMarketId, OUT int32 &iPosFound, OUT CMarket *&pMarketFound);

	// ����ָ���������Ʒ
	bool32			FindMerch(IN const CString &StrMerchCode, OUT CMerch *&pMerchFound);

	// ����ָ������ľ���ϯλ
	bool32			FindBroker(IN const CString &StrBrokerCode, OUT int32 &iPosFound, OUT CBroker *&pBrokerFound);

	// ����һ���г������ж��Ƿ����ظ���
	bool32			AddMarket(CMarket *pMarket);

	// ����һ������ϯλ�����ж��Ƿ����ظ���
	bool32			AddBroker(CMmiBroker &BrokerSrc);

public:
	int32			m_iBreedId;				// ����Ʒ�ֱ�ţ� �磺���Ϻ�֤ȯ�г������Ϊ0
	int32			m_iShowId;				// ��ʾ��˳��
	CString			m_StrBreedCnName;		// ����Ʒ�����ƣ� �磺���Ϻ�֤ȯ�г����� �����֤ȯ�г����� ��ȫ������г���������
	CString			m_StrBreedEnName;

	CArray<CMarket*, CMarket*> m_MarketListPtr;	// �г��б�
	std::map<int32, int32> m_aMapMarkets;	// ������ٲ����г�
	
	CArray<CBroker*, CBroker*> m_BrokersPtr;// ���г������еľ���ϯλ	// zhangbo 0326 #���Ż����ô�����������Ǿ�������ģ� ���й��ڸ����ݵ���ز������Զ����ö��ֲ��ҵ��㷨���ӿ���ҹ���
	std::map<CString, int32> m_aMapBrokers;	// ������ٲ��Ҿ�����
};

// 
// /////////////////////////////////////////////////////////////
// // �����Ʒ
class DATAINFO_DLL_EXPORT CSiblingMerch
{
public:
	CSiblingMerch() { m_pMerch = NULL; }

public:
	CMerch			*m_pMerch;
	
	//
	CString			m_StrSiblingName;
};

class DATAINFO_DLL_EXPORT CRelativeMerchNode
{
public: 
	const CRelativeMerchNode& operator=(const CRelativeMerchNode &Src);

public:
	CString			m_RelativeName;			// ��ϵ����
	CString			m_RelativeSummary;		// ��ϵ����
	
	CArray<CSiblingMerch, CSiblingMerch&>	m_SiblingMerchList;	// ��Ʒ�б�
};


#endif


