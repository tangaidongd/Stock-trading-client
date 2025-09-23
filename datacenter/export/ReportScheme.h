#ifndef _REPORT_SCHEME_H_
#define _REPORT_SCHEME_H_

#define GHCOUNTS	 67
#define MAXWIDTH	 500
#define DEFUALTWIDTH 70
#include "typedef.h"
#include "Enumtype.h"
#include "DataCenterExport.h"

struct T_ReportHeaderObject;
struct T_ReportTypeObject;
struct T_HeadInfo;

class CGGTongDoc;
// ϵͳ���۱���ͷ
class DATACENTER_DLL_EXPORT CReportScheme
{
public:	
	// ���б�ͷ����
	// ���۱���ͷ�б�
	enum E_ReportHeader 
	{
		ERHRowNo = 0,			// �к�
		ERHMerchCode,			// ����
		ERHMerchName,			// ����

		// 
		ERHPricePrevClose,		// ���ռ�
		ERHPricePrevBalance,	// ����գ��ڻ������ƣ� ��ͬ�ڹ�Ʊ�����ռۣ�

		// 
		ERHPriceOpen,			// ���̼�

		// 
		ERHPriceNew,			// ���¼�
	 // ERHPriceClose,			// ���̼ۣ�=���¼ۣ�
		ERHPriceBalance,		// ����ۣ��ڻ������ƣ� =���̼ۣ�

		// 
		ERHPriceHigh,			// ��� 
		ERHPriceLow,			// ���

		//
		ERHVolumeCur,			// ����
		//
		ERHVolumeTotal,			// �ɽ���
		// 
		ERHAmount,				// ���
	 	ERHHold,				// �ֲ� 
		ERHRiseFall,			// �ǵ�
		ERHRange,				// �Ƿ�%
		ERHSwing,				// ���%
		ERHVolumeRate,			// ����
		ERHPowerDegree,			// ǿ����
		ERHDKBallance,			// ���ƽ��
		ERHDWin,				// ��ͷ����
		ERHDLose,				// ��ͷͣ��
		ERHKWin,				// ��ͷ�ز�
		ERHKLose,				// ��ͷͣ��
		ERHRate,				// ί��
		ERHSpeedRiseFall,		// �����ǵ�
		ERHChangeRate,			// ������
		ERHMarketWinRate,		// ��ӯ��		
		ERHBuyPrice,			// �����
		ERHSellPrice,			// ������
		ERHBuyAmount,			// ������
		ERHSellAmount,			// ������
		ERHDifferenceHold,		// �ֲֲ�
		ERHTime,				// ʱ��
		ERHBuySellPrice,		// �����/������
		ERHBuySellVolume,		// ��/����
		ERHPreDone1,			// ǰ��1
		ERHPreDone2,			// ǰ��2
		ERHPreDone3,			// ǰ��3
		ERHPreDone4,			// ǰ��4
		ERHPreDone5,			// ǰ��5
		
		// fangz0609 Add for Warrants
		ERHTargetStockCode,		// ��Ĺ�
		ERHTargetStockName,		// ��Ĺ���
		ERHSellOrBuy,			// �Ϲ��Ϲ�
		ERHXQRate,				// ��Ȩ��
		ERHXQPrice,				// ��Ȩ��
		ERHMaturity,			// ������
		ERHDuration,			// ������
		ERHTheoreticalPrice,	// ���ۼ�
		ERHPremium,				// ���
		ERHUsaOrEuro,			// ŷʽ��ʽ
		ERHCirculateBallance,	// ��ͨ���
		ERHDistributors,		// ������
		ERHWarrantType,			// Ȩ֤����
		ERHPriceInOut,			// ���ڼ���
		ERHValueInside,			// ���ڼ�ֵ
		ERHAmplitudeHistory,	// ��ʷ����
		ERHAmplitudeExplicate,  // ���첨��
		ERHDelta,				// �Գ�ֵ
		ERHGAMMA,				// GAMMA
		ERHLeverRate,			// �ܸ˱���
		ERHLeverTrue,			// ʵ�ʸܸ�
		ERHStockChangeRate,		// ���ɱ���
		ERHVolumeWildGoods,  	// �ֻ���
		ERHRateWildGoods,		// �ֻ�����

		// fangz0811 Add for Futures
		ERHBuild,				// ����
		ERHClear,				// ƽ��
		ERHBuildPer,			// ���ʿ���
		ERHClearPer,			// ����ƽ��
		ERHAddPer,				// ��������
		ERHBuyVolumn,			// ����
		ERHSellVolumn,			// ����
		ERHTrend,				// ����

		// xl 0813 ������������ѡ��
		ERHShortLineSelect,		// ����ѡ��
		ERHMidLineSelect,		// ����ѡ��
		ERHCapitalFlow,			// �ʽ�����

		// xl 0820 ��������
		ERHAllCapital,			// �ܹɱ�
		ERHCircAsset,			// ��ͨ�ɱ�
		ERHAllAsset,			// ���ʲ�
		ERHFlowDebt,			// ������ծ
		ERHPerFund,				// ÿ�ɹ�����

		ERHBusinessProfit,		// Ӫҵ����
		ERHPerNoDistribute,		// ÿ��δ����
		ERHPerIncomeYear,		// ÿ������(��)
		ERHPerPureAsset,		// ÿ�ɾ��ʲ�
		ERHChPerPureAsset,		// ����ÿ�ɾ��ʲ�

		ERHDorRightRate,		// �ɶ�Ȩ���// 
		ERHCircMarketValue,		// ��ͨ��ֵ
		ERHAllMarketValue,		// ����ֵ

		// xl 0820 ����ȣ�
		ERHBuySellRate,			// �����

		// fangz 1015
		ERHPriceSell5,			// ��5��
		ERHPriceSell4,			// ��4��
		ERHPriceSell3,			// ��3��
		ERHPriceSell2,			// ��2��
		ERHPriceSell1,			// ��1��

		ERHVolSell5,			// ��5��
		ERHVolSell4,			// ��4��
		ERHVolSell3,			// ��3��
		ERHVolSell2,			// ��2��
		ERHVolSell1,			// ��1��

		ERHPriceBuy5,			// ��5��
		ERHPriceBuy4,			// ��4��
		ERHPriceBuy3,			// ��3��
		ERHPriceBuy2,			// ��2��
		ERHPriceBuy1,			// ��1��

		ERHVolBuy5,				// ��5��
		ERHVolBuy4,				// ��4��
		ERHVolBuy3,				// ��3��
		ERHVolBuy2,				// ��2��
		ERHVolBuy1,				// ��1��

		ERHIndustry,			// ������ҵ
		ERHZixuan,				// ��ѡ

		// 
		ERHAllNetAmount,		// ������
		ERHExBigBuyAmount,		// ��������
		ERHExBigSellAmount,		// ��������
		ERHExBigNetAmount,		// ���󵥾���
		ERHExBigNetAmountProportion,		// ���󵥾���ռ�� 
		ERHExBigNetTotalAmount,				// �����ܶ�
		ERHExBigNetTotalAmountProportion,	// �����ܶ�ռ��
		
		//
		ERHBigBuyAmount,		// ������
		ERHBigSellAmount,		// ������
		ERHBigNetAmount,		// �󵥾���
		ERHBigNetAmountProportion,			// �󵥾���ռ�� 
		ERHBigNetTotalAmount,				// ���ܶ�
		ERHBigNetTotalAmountProportion,		// ���ܶ�ռ��
		
		//
		ERHMidBuyAmount,		// �е�����
		ERHMidSellAmount,		// �е�����
		ERHMidNetAmount,		// �е�����
		ERHMidNetAmountProportion,			// �е�����ռ�� 
		ERHMidNetTotalAmount,				// �е��ܶ�
		ERHMidNetTotalAmountProportion,		// �е��ܶ�ռ��
		
		//
		ERHSmallBuyAmount,		// С������
		ERHSmallSellAmount,		// С������
		ERHSmallNetAmount,	    // С������
		ERHSmallNetAmountProportion,		// С������ռ�� 
		ERHSmallNetTotalAmount,				// С���ܶ�
		ERHSmallNetTotalAmountProportion,	// С���ܶ�ռ��
		
		//
		ERHSelStockTime,	    // ѡ��ʱ��
		ERHSelPrice,            // ��ѡ�۸�
		ERHSelRisePercent,      // ��ѡ���Ƿ�%

		ERHTodayMasukuraProportion,		// ��������ռ��
		ERHTodayRanked,					// ��������
		ERHTodayRise,					// �����Ƿ�

		ERH2DaysMasukuraProportion,		// 2������ռ��
		ERH2DaysRanked,					// 2������
		ERH2DaysRise,					// 2���Ƿ�

		ERH3DaysMasukuraProportion,		// 3������ռ��
		ERH3DaysRanked,					// 3������
		ERH3DaysRise,					// 3���Ƿ�

		ERH5DaysMasukuraProportion,		// 5������ռ��
		ERH5DaysRanked,					// 5������
		ERH5DaysRise,					// 5���Ƿ�

		ERH10DaysMasukuraProportion,	// 10������ռ��
		ERH10DaysRanked,				// 10������
		ERH10DaysRise,					// 10���Ƿ�

		// ��Чֵ����
		ERHMarketMargin,				// �о���
		ERHNetProfit,					// ������
		ERHTotalProfit,					// �����ܶ�
		ERHassetsRate,					// ���ʲ�������	
		ERHNetProfitGrowthRate,			// ������������
		ERHNetAmountofthemain,			// ��������
		
		ERHCount
	};	

public:
	CReportScheme(); 
	~CReportScheme();
	void Construct(CString strPrivateFolderName);
public:
	static CReportScheme*	Instance();	// ���������һ������
	
public:
	static bool32	IsFuture(E_ReportType eReportType) 
	{
		if (  ERTFuturesCn == eReportType
		   || ERTFuturesForeign == eReportType
		   || ERTFutureRelaMonth == eReportType
		   || ERTMony == eReportType
		   || ERTExpForeign == eReportType 
		   || ERTFutureGold == eReportType ) 	  
		{
			return true;
		}
		
		return false;
	}

public:
	void			GetReportHeadInfoList(E_ReportType eReportType,OUT CArray<T_HeadInfo,T_HeadInfo>& aHeadInfo,int32& iFixCol);
	void			GetHead(E_ReportType eReportType,OUT CStringArray& aStrGridHead,OUT CStringArray& aStrAnotherGridHead);
	void			GetHead(IN CStringArray& aStrGridHead,OUT CStringArray& aStrAnotherGridHead,E_ReportType eReportType);
	
	void			ChangeHeadInfoData(E_ReportType eReportType,CArray<T_HeadInfo,T_HeadInfo>& aHeadInfo,int32& iFixCol);
	bool32			ReadDataFromXml();
	bool32			SaveDataToXml();
	void			SetHeadDataToDefault(E_ReportType eReportType,CArray<T_HeadInfo,T_HeadInfo>& aHeadInfo,int32& iFixCol);
	void			SetBuySellPriceInfo(CString strBuyPrice, CString strSellPrice, CString strNewPrice);
		
	void			SaveColumnWidth(E_ReportType eReportType,int32 iColumn,int32 iWidth);
	void			DoHeadInfoString(CString StrHeadInfo,OUT CString& StrHeadName,OUT int32& iWidth);
	CString			HeadCnName2SummaryName(CString StrCnName);
	CString			HeadSummaryName2CnName(CString StrSummaryName);
	static CString	GetReportHeaderCnName(E_ReportHeader eReportHeader);
	static CString	GetReportTypeCnName(E_ReportType e_ReportType);	
	CString			GetPath();	
	static CString  GetFixColAttriName();
	static			CReportScheme::E_ReportHeader  GetReportHeaderEType(CString StrHeadCnName);
	void			SetPrivateFolderName(CString& strName);
	static int		GetReportColDefaultWidth(CReportScheme::E_ReportHeader eHeaderType);
	CString			HeadSpecialCnName2HeadCnName(CString strName);
	static CString	GetSpecialReportHeaderCnName(CString strName);

private:
	CArray<T_HeadInfo,T_HeadInfo>		m_aReportHeadInfo[ERTCount];
	int32								m_aFixCol[ERTCount];		
private:
	static CReportScheme	*s_Instance;
	CString	                m_strPrivateName;	
};

typedef struct T_ReportHeaderObject
{
	CReportScheme::E_ReportHeader	m_eReportHeader;
	CString			m_StrCnName;
	CString			m_StrEnName;
	CString			m_StrSummary;
			
 	T_ReportHeaderObject(const CReportScheme::E_ReportHeader eReportHeader, const CString &StrCnName, const CString &StrEnName, const CString &StrSummary)
 	{
		m_eReportHeader = eReportHeader;

 		m_StrCnName		= StrCnName;
 		m_StrEnName		= StrEnName;
		m_StrSummary	= StrSummary;	
	};

}T_ReportHeaderObject;

typedef struct T_ReportTypeObject
{
	E_ReportType		m_eReportType;
	CString			m_StrCnName;
	CString			m_StrEnName;
	CString			m_StrSummary;
	T_ReportTypeObject(const E_ReportType eReportType,const CString & StrCnName,const CString & StrEnName)
	{
		m_eReportType = eReportType;
		
		m_StrCnName	  = StrCnName;
		m_StrEnName   = StrEnName;
	};

}T_ReportTypeObject;

typedef struct T_HeadInfo 
{
	CString m_StrHeadNameCn;
	int32	m_iHeadWidth;
	CReportScheme::E_ReportHeader m_eReportHeader;

public:	
	T_HeadInfo(){ m_eReportHeader = CReportScheme::ERHCount; };

	T_HeadInfo(const CString& StrHeadNameCn, const int32 iHeadWidth)
	{
		m_StrHeadNameCn = StrHeadNameCn;
		m_iHeadWidth	= iHeadWidth;
		m_eReportHeader = CReportScheme::GetReportHeaderEType(m_StrHeadNameCn);
		ASSERT( m_eReportHeader != CReportScheme::ERHCount );
	};
	bool32 InitilizeHeadInfo(CReportScheme::E_ReportHeader eHeader);
}T_HeadInfo;


#endif
