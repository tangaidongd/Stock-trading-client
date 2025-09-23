#include "StdAfx.h"
#include "ReportScheme.h"
#include "coding.h"
#include <map>
using std::map;

CReportScheme * CReportScheme::s_Instance = NULL;
CString	m_strBuyPrice, m_strSellPrice, m_strNewPrice;

//////////////////////////////////////////////////////////////////////////
// XML
const char*	KStrElementNameHeadInfo				=	"HeadInfo";
const char*	KStrElementNameHeadItem				=	"HeadItem";
const char*	KStrElementAttriHeadType			=	"Type";
const char*	KStrElementAttriFixCol				=	"FixCol";
const char*	KStrElementAttriHeadName			=	"Name";
const char*	KStrElementAttriHeadWidth			=	"Width";
const char*	KStrElementAttriHeadColumnType		=	"HCType";	// �����ƶ�Ӧ�����ͣ���������Ҳ��������������type����

////////////////////////////////////////////////////////////////////////
// �����б�, �ñ���E_ReportHeaderһһ��Ӧ
T_ReportHeaderObject s_astReportHeaderObjects[] = 
{
	T_ReportHeaderObject(CReportScheme::ERHRowNo,				L"��",					L"RowNo",				L"��"),
	T_ReportHeaderObject(CReportScheme::ERHMerchCode,			L"����",				L"Code",				L"����"),
	T_ReportHeaderObject(CReportScheme::ERHMerchName,			L"����",				L"Name",				L"����"),
	T_ReportHeaderObject(CReportScheme::ERHPricePrevClose,		L"����",				L"PrevClose",			L"����"),	
	T_ReportHeaderObject(CReportScheme::ERHPricePrevBalance,	L"���",				L"PrevBalance",			L"���"),
	T_ReportHeaderObject(CReportScheme::ERHPriceOpen,			L"����",				L"PriceOpen",			L"����"),
	T_ReportHeaderObject(CReportScheme::ERHPriceNew,			L"����", 				L"PriceNew",			L"����"),
	T_ReportHeaderObject(CReportScheme::ERHPriceBalance,		L"����",				L"PriceBalance",		L"����"),
	T_ReportHeaderObject(CReportScheme::ERHPriceHigh,			L"���",				L"PriceHigh",			L"���"),
	T_ReportHeaderObject(CReportScheme::ERHPriceLow,			L"���",				L"PriceLow",			L"���"),
	T_ReportHeaderObject(CReportScheme::ERHVolumeCur,			L"����",				L"VolumeCur",			L"����"),
	T_ReportHeaderObject(CReportScheme::ERHVolumeTotal,			L"����",				L"VolumeTotal",			L"����"),
	T_ReportHeaderObject(CReportScheme::ERHAmount,				L"�ܽ��",				L"Amount",				L"�ܽ��"),
 	T_ReportHeaderObject(CReportScheme::ERHHold,				L"�ֲ���",				L"Hold",				L"�ֲ���"),
	T_ReportHeaderObject(CReportScheme::ERHRiseFall,			L"�ǵ�",				L"RiseFall",			L"�ǵ�"),
	T_ReportHeaderObject(CReportScheme::ERHRange ,				L"�Ƿ�%",				L"Range ",				L"�Ƿ�%"),
	T_ReportHeaderObject(CReportScheme::ERHSwing ,				L"���%",				L"Swing ",				L"���%"),
	T_ReportHeaderObject(CReportScheme::ERHVolumeRate,			L"����",				L"VolumeRate ",			L"����"),
	T_ReportHeaderObject(CReportScheme::ERHPowerDegree,			L"ǿ����%",				L"PowerDegree ",		L"ǿ����%"),
	T_ReportHeaderObject(CReportScheme::ERHDKBallance,			L"���ƽ��",			L"DKBallance ",			L"���ƽ��"),
	T_ReportHeaderObject(CReportScheme::ERHDWin,				L"��ͷ����",			L"DWin ",				L"��ͷ����"),
	T_ReportHeaderObject(CReportScheme::ERHDLose,				L"��ͷͣ��",			L" DLose",				L"��ͷͣ��"),
	T_ReportHeaderObject(CReportScheme::ERHKWin,				L"��ͷ�ز�",			L" KWin",				L"��ͷ�ز�"),
	T_ReportHeaderObject(CReportScheme::ERHKLose,				L"��ͷͣ��",			L"KLose ",				L"��ͷͣ��"),
	T_ReportHeaderObject(CReportScheme::ERHRate,				L"ί��%",				L"Rate ",				L"ί��%"),
	T_ReportHeaderObject(CReportScheme::ERHSpeedRiseFall,		L"����%",				L"SpeedRiseFall ",		L"����%"),
	T_ReportHeaderObject(CReportScheme::ERHChangeRate,			L"������%",				L"ChangeRate ",			L"������%"),
	T_ReportHeaderObject(CReportScheme::ERHMarketWinRate,		L"��ӯ��",				L"MarketWinRate ",		L"��ӯ��"),
	T_ReportHeaderObject(CReportScheme::ERHBuyPrice,			L"���",				L"BuyPrice ",			L"�����"),
	T_ReportHeaderObject(CReportScheme::ERHSellPrice,			L"����",				L"SellPrice ",			L"������"),
	T_ReportHeaderObject(CReportScheme::ERHBuyAmount,			L"����",				L"BuyAmount ",			L"������"),
	T_ReportHeaderObject(CReportScheme::ERHSellAmount,			L"����",				L"SellAmount ",			L"������"),
	T_ReportHeaderObject(CReportScheme::ERHDifferenceHold,		L"�ֲ�",				L"DifferenceHold ",		L"�ֲֲ�"),
	T_ReportHeaderObject(CReportScheme::ERHTime,				L"ʱ��",				L"Time ",				L"ʱ��"),
	T_ReportHeaderObject(CReportScheme::ERHBuySellPrice ,		L"���/����",			L" BuySellPrice",		L"�����/������ "),
	T_ReportHeaderObject(CReportScheme::ERHBuySellVolume ,		L"��/����",				L"BuySellVolume ",		L"��/���� "),
	T_ReportHeaderObject(CReportScheme::ERHPreDone1 ,			L"ǰ��1",				L"PreDone1 ",			L"ǰ��һ"),
	T_ReportHeaderObject(CReportScheme::ERHPreDone2 ,			L"ǰ��2",				L"PreDone2 ",			L"ǰ�ɶ�"),
	T_ReportHeaderObject(CReportScheme::ERHPreDone3 ,			L"ǰ��3",				L"PreDone3 ",			L"ǰ����"),
	T_ReportHeaderObject(CReportScheme::ERHPreDone4 ,			L"ǰ��4",				L"PreDone4 ",			L"ǰ����"),
	T_ReportHeaderObject(CReportScheme::ERHPreDone5 ,			L"ǰ��5",				L"PreDone5 ",			L"ǰ����"),
	//////////////////////////////////////////////////////////////////////////
	// fangz0609 Add for Warrants44

	T_ReportHeaderObject(CReportScheme::ERHTargetStockCode,		L"��Ĺ�",				L"TargetStockCode",		L"��Ĺ�"),
	T_ReportHeaderObject(CReportScheme::ERHTargetStockName ,	L"��Ĺ���",			L"TargetStockName",		L"��Ĺ���"),
	T_ReportHeaderObject(CReportScheme::ERHSellOrBuy ,			L"�Ϲ��Ϲ�",			L"SellOrBuy",			L"�Ϲ��Ϲ�"),
	T_ReportHeaderObject(CReportScheme::ERHXQRate ,				L"��Ȩ��",				L"XQRate",				L"��Ȩ��"),
	T_ReportHeaderObject(CReportScheme::ERHXQPrice ,			L"��Ȩ��",				L"XQPrice",				L"��Ȩ��"),
	T_ReportHeaderObject(CReportScheme::ERHMaturity ,			L"������",				L"Maturity",			L"������"),
	T_ReportHeaderObject(CReportScheme::ERHDuration ,			L"������",				L"Duration",			L"������"),
	T_ReportHeaderObject(CReportScheme::ERHTheoreticalPrice ,	L"���ۼ�",				L"TheoreticalPrice",	L"���ۼ�"),
	T_ReportHeaderObject(CReportScheme::ERHPremium ,			L"���",				L"Premium",				L"���"),
	T_ReportHeaderObject(CReportScheme::ERHUsaOrEuro ,			L"ŷʽ��ʽ",			L"UsaOrEuro",			L"ŷʽ��ʽ"),
	T_ReportHeaderObject(CReportScheme::ERHCirculateBallance ,	L"��ͨ���",			L"CirculateBallance",	L"��ͨ���"),
	T_ReportHeaderObject(CReportScheme::ERHDistributors ,		L"������",				L"Distributors",		L"������"),
	T_ReportHeaderObject(CReportScheme::ERHWarrantType ,		L"Ȩ֤����",			L"WarrantType",			L"Ȩ֤����"),
	T_ReportHeaderObject(CReportScheme::ERHPriceInOut ,			L"���ڼ���",			L"PriceInOut",			L"���ڼ���"),
	T_ReportHeaderObject(CReportScheme::ERHValueInside ,		L"���ڼ�ֵ",			L"ValueInside",			L"���ڼ�ֵ"),
	T_ReportHeaderObject(CReportScheme::ERHAmplitudeHistory ,	L"��ʷ����",			L"AmplitudeHistory",	L"��ʷ����"),
	T_ReportHeaderObject(CReportScheme::ERHAmplitudeExplicate ,	L"���첨��",			L"AmplitudeExplicate",	L"���첨��"),
	T_ReportHeaderObject(CReportScheme::ERHDelta ,				L"�Գ�ֵ",				L"Delta",				L"�Գ�ֵ"),
	T_ReportHeaderObject(CReportScheme::ERHGAMMA ,				L"GAMMA",				L"GAMMA",				L"GAMMA"),
	T_ReportHeaderObject(CReportScheme::ERHLeverRate ,			L"�ܸ˱���",			L"LeverRate",			L"�ܸ˱���"),
	T_ReportHeaderObject(CReportScheme::ERHLeverTrue ,			L"ʵ�ʸܸ�",			L"LeverTrue",			L"ʵ�ʸܸ�"),
	T_ReportHeaderObject(CReportScheme::ERHStockChangeRate ,	L"���ɱ���",			L"StockChangeRate",		L"���ɱ���"),
	T_ReportHeaderObject(CReportScheme::ERHVolumeWildGoods ,	L"�ֻ���",				L"VolumeWildGoods",		L"�ֻ���"),
	T_ReportHeaderObject(CReportScheme::ERHRateWildGoods ,		L"�ֻ�����",			L"RateWildGoods",		L"�ֻ�����"),

	//////////////////////////////////////////////////////////////////////////
	// fangz0811 Add for Futures
	T_ReportHeaderObject(CReportScheme::ERHBuild ,				L"����",				L"Build",				L"����"),
	T_ReportHeaderObject(CReportScheme::ERHClear ,				L"ƽ��",				L"Clear",				L"ƽ��"),
	T_ReportHeaderObject(CReportScheme::ERHBuildPer ,			L"����",				L"BuildPer",			L"����"),
	T_ReportHeaderObject(CReportScheme::ERHClearPer ,			L"��ƽ",				L"ClearPer",			L"��ƽ"),
	T_ReportHeaderObject(CReportScheme::ERHAddPer ,				L"����",				L"AddPer",				L"����"),
	T_ReportHeaderObject(CReportScheme::ERHBuyVolumn ,			L"����",				L"BuyVolumn",			L"����"),
	T_ReportHeaderObject(CReportScheme::ERHSellVolumn ,			L"����",				L"SellVolumn",			L"����"),
	T_ReportHeaderObject(CReportScheme::ERHTrend ,				L"����",				L"Trend",				L"����"),

	//////////////////////////////////////////////////////////////////////////
	// xl 0813 ��������ѡ���ֶ�
	T_ReportHeaderObject(CReportScheme::ERHShortLineSelect ,	L"����",				L"ShortLineSelect",		L"����"),
	T_ReportHeaderObject(CReportScheme::ERHMidLineSelect ,		L"����",				L"MidLineSelect",		L"����"),
	T_ReportHeaderObject(CReportScheme::ERHCapitalFlow ,		L"�ʽ�����",			L"CapitalFlow",			L"�ʽ�����"),

	//////////////////////////////////////////////////////////////////////////
	// xl 0820 ��������
	T_ReportHeaderObject(CReportScheme::ERHAllCapital ,			L"�ܹɱ�(��)",			L"AllCapital",			L"�ܹɱ�(��)"),
	T_ReportHeaderObject(CReportScheme::ERHCircAsset ,			L"��ͨ�ɱ�(��)",		L"CircAsset",			L"��ͨ�ɱ�(��)"),
	T_ReportHeaderObject(CReportScheme::ERHAllAsset ,			L"���ʲ�(��)",			L"AllAsset",			L"���ʲ�(��)"),
	T_ReportHeaderObject(CReportScheme::ERHFlowDebt ,			L"������ծ(��)",		L"FlowDebt",			L"������ծ(��)"),
	T_ReportHeaderObject(CReportScheme::ERHPerFund ,			L"ÿ�ɹ�����",			L"PerFund",				L"ÿ�ɹ�����"),
	T_ReportHeaderObject(CReportScheme::ERHBusinessProfit ,		L"Ӫҵ����(��)",		L"BusinessProfit",		L"Ӫҵ����(��)"),
	T_ReportHeaderObject(CReportScheme::ERHPerNoDistribute ,	L"ÿ��δ����",			L"PerNoDistribute",		L"ÿ��δ����"),
	T_ReportHeaderObject(CReportScheme::ERHPerIncomeYear ,		L"ÿ������",		L"PerIncomeYear",		L"ÿ������"),
	T_ReportHeaderObject(CReportScheme::ERHPerPureAsset ,		L"ÿ�ɾ��ʲ�",			L"PerPureAsset",		L"ÿ�ɾ��ʲ�"),
	T_ReportHeaderObject(CReportScheme::ERHChPerPureAsset ,		L"����ÿ�ɾ��ʲ�",		L"ChPerPureAsset",		L"����ÿ�ɾ��ʲ�"),
	T_ReportHeaderObject(CReportScheme::ERHDorRightRate ,		L"�ɶ�Ȩ���",			L"DorRightRate",		L"�ɶ�Ȩ���"),
	T_ReportHeaderObject(CReportScheme::ERHCircMarketValue ,	L"��ͨ��ֵ(��)",		L"CircMarketValue",		L"��ͨ��ֵ(��)"),
	T_ReportHeaderObject(CReportScheme::ERHAllMarketValue ,		L"����ֵ(��)",			L"AllMarketValue",		L"����ֵ(��)"),

	T_ReportHeaderObject(CReportScheme::ERHBuySellRate ,		L"�����",				L"BuySellRate",			L"�����"),

	//////////////////////////////////////////////////////////////////////////
	// fangz 1015
	T_ReportHeaderObject(CReportScheme::ERHPriceSell5 ,			L"���ݼ�",				L"PriceSell5",			L"���ݼ�"),
	T_ReportHeaderObject(CReportScheme::ERHPriceSell4 ,			L"���ܼ�",				L"PriceSell4",			L"���ܼ�"),
	T_ReportHeaderObject(CReportScheme::ERHPriceSell3 ,			L"���ۼ�",				L"PriceSell3",			L"���ۼ�"),
	T_ReportHeaderObject(CReportScheme::ERHPriceSell2 ,			L"���ڼ�",				L"PriceSell2",			L"���ڼ�"),
	T_ReportHeaderObject(CReportScheme::ERHPriceSell1 ,			L"���ټ�",				L"PriceSell1",			L"���ټ�"),

	T_ReportHeaderObject(CReportScheme::ERHVolSell5 ,			L"������",				L"VolSell5",			L"������"),
	T_ReportHeaderObject(CReportScheme::ERHVolSell4 ,			L"������",				L"VolSell4",			L"������"),
	T_ReportHeaderObject(CReportScheme::ERHVolSell3 ,			L"������",				L"VolSell3",			L"������"),
	T_ReportHeaderObject(CReportScheme::ERHVolSell2 ,			L"������",				L"VolSell2",			L"������"),
	T_ReportHeaderObject(CReportScheme::ERHVolSell1 ,			L"������",				L"VolSell1",			L"������"),

	T_ReportHeaderObject(CReportScheme::ERHPriceBuy5 ,			L"��ݼ�",				L"PriceBuy5",			L"��ݼ�"),
	T_ReportHeaderObject(CReportScheme::ERHPriceBuy4 ,			L"��ܼ�",				L"PriceBuy4",			L"��ܼ�"),
	T_ReportHeaderObject(CReportScheme::ERHPriceBuy3 ,			L"��ۼ�",				L"PriceBuy3",			L"��ۼ�"),
	T_ReportHeaderObject(CReportScheme::ERHPriceBuy2 ,			L"��ڼ�",				L"PriceBuy2",			L"��ڼ�"),
	T_ReportHeaderObject(CReportScheme::ERHPriceBuy1 ,			L"��ټ�",				L"PriceBuy1",			L"��ټ�"),

	T_ReportHeaderObject(CReportScheme::ERHVolBuy5 ,			L"�����",				L"VolBuy5",				L"�����"),
	T_ReportHeaderObject(CReportScheme::ERHVolBuy4 ,			L"�����",				L"VolBuy4",				L"�����"),
	T_ReportHeaderObject(CReportScheme::ERHVolBuy3 ,			L"�����",				L"VolBuy3",				L"�����"),
	T_ReportHeaderObject(CReportScheme::ERHVolBuy2 ,			L"�����",				L"VolBuy2",				L"�����"),
	T_ReportHeaderObject(CReportScheme::ERHVolBuy1 ,			L"�����",				L"VolBuy1",				L"�����"),
	T_ReportHeaderObject(CReportScheme::ERHIndustry,		    L"������ҵ",			L"Industry",			L"������ҵ"),
	T_ReportHeaderObject(CReportScheme::ERHZixuan,				L"��ѡ",				L"ZiXuan",				L"��ѡ"),
	T_ReportHeaderObject(CReportScheme::ERHAllNetAmount,		L"������",				L"AllNetAmount",		L"������"),
	T_ReportHeaderObject(CReportScheme::ERHExBigBuyAmount,		L"��������",			L"ExBigBuyAmount",		L"��������"),
	T_ReportHeaderObject(CReportScheme::ERHExBigSellAmount,		L"��������",			L"ExBigSellAmount",		L"��������"),
	T_ReportHeaderObject(CReportScheme::ERHExBigNetAmount,		L"���󵥾���",			L"ExBigNetAmount",		L"���󵥾���"),
	T_ReportHeaderObject(CReportScheme::ERHExBigNetAmountProportion,	 L"���󵥾���ռ��(%)",	L"ExBigNetAmountProportion",		L"���󵥾���ռ��(%)"),
	T_ReportHeaderObject(CReportScheme::ERHExBigNetTotalAmount,			 L"�����ܶ�",			L"ExBigNetTotalAmount",				L"�����ܶ�"),
	T_ReportHeaderObject(CReportScheme::ERHExBigNetTotalAmountProportion,L"�����ܶ�ռ��(%)",	L"ExBigNetTotalAmountProportion",	L"�����ܶ�ռ��(%)"),
	
	T_ReportHeaderObject(CReportScheme::ERHBigBuyAmount,		L"������",				L"BigBuyAmount",				L"������"),
	T_ReportHeaderObject(CReportScheme::ERHBigSellAmount,		L"������",				L"BigSellAmount",				L"������"),
	T_ReportHeaderObject(CReportScheme::ERHBigNetAmount,		L"�󵥾���",				L"BigNetAmount",				L"�󵥾���"),
	T_ReportHeaderObject(CReportScheme::ERHBigNetAmountProportion,	L"�󵥾���ռ��(%)",		L"BigNetAmountProportion",		L"�󵥾���ռ��(%)"),
	T_ReportHeaderObject(CReportScheme::ERHBigNetTotalAmount,	L"���ܶ�",				L"BigNetTotalAmount",			L"���ܶ�"),
	T_ReportHeaderObject(CReportScheme::ERHBigNetTotalAmountProportion,	L"���ܶ�ռ��(%)",	L"BigNetTotalAmountProportion",	L"���ܶ�ռ��(%)"),

	T_ReportHeaderObject(CReportScheme::ERHMidBuyAmount,		L"�е�����",				L"MidBuyAmount",					L"�е�����"),
	T_ReportHeaderObject(CReportScheme::ERHMidSellAmount,		L"�е�����",				L"MidSellAmount",					L"�е�����"),
	T_ReportHeaderObject(CReportScheme::ERHMidNetAmount,		L"�е�����",				L"MidNetAmount",					L"�е�����"),
	T_ReportHeaderObject(CReportScheme::ERHMidNetAmountProportion,	L"�е�����ռ��(%)",		L"MidNetAmountProportion",			L"�е�����ռ��(%)"),
	T_ReportHeaderObject(CReportScheme::ERHMidNetTotalAmount,	L"�е��ܶ�",				L"MidNetTotalAmount",				L"�е��ܶ�"),
	T_ReportHeaderObject(CReportScheme::ERHMidNetTotalAmountProportion,	L"�е��ܶ�ռ��(%)",	L"MidNetTotalAmountProportion",		L"�е��ܶ�ռ��(%)"),

	T_ReportHeaderObject(CReportScheme::ERHSmallBuyAmount,					L"С������",			L"SmallBuyAmount",					L"С������"),
	T_ReportHeaderObject(CReportScheme::ERHSmallSellAmount,					L"С������",			L"SmallSellAmount",					L"С������"),
	T_ReportHeaderObject(CReportScheme::ERHSmallNetAmount,					L"С������",			L"SmallNetAmount",					L"С������"),
	T_ReportHeaderObject(CReportScheme::ERHSmallNetAmountProportion,		L"С������ռ��(%)",		L"SmallNetAmountProportion",		L"С������ռ��(%)"),
	T_ReportHeaderObject(CReportScheme::ERHSmallNetTotalAmount,				L"С���ܶ�",			L"SmallNetTotalAmount",				L"С���ܶ�"),
	T_ReportHeaderObject(CReportScheme::ERHSmallNetTotalAmountProportion,	L"С���ܶ�ռ��(%)",		L"SmallNetTotalAmountProportion",	L"С���ܶ�ռ��(%)"),
	
	T_ReportHeaderObject(CReportScheme::ERHSelStockTime,	    L"��ѡʱ��",			L"SelStockTime",			L"��ѡʱ��"),
	T_ReportHeaderObject(CReportScheme::ERHSelPrice,	        L"��ѡ�۸�",			L"SelectPrice",				L"��ѡ�۸�"),
	T_ReportHeaderObject(CReportScheme::ERHSelRisePercent,	    L"��ѡ���Ƿ�(%)",		L"SelectRisePercent",		L"��ѡ���Ƿ�(%)"),

	T_ReportHeaderObject(CReportScheme::ERHTodayMasukuraProportion,	L"��������ռ��(%)",	L"TodayMasukuraProportion",	L"��������ռ��(%)"),
	T_ReportHeaderObject(CReportScheme::ERHTodayRanked,				L"��������",		L"TodayRanked",				L"��������"),
	T_ReportHeaderObject(CReportScheme::ERHTodayRise,				L"�����Ƿ�(%)",		L"TodayRise",				L"�����Ƿ�(%)"),
	T_ReportHeaderObject(CReportScheme::ERH2DaysMasukuraProportion,	L"2������ռ��(%)",	L"2DaysMasukuraProportion",	L"2������ռ��(%)"),
	T_ReportHeaderObject(CReportScheme::ERH2DaysRanked,				L"2������",			L"2DaysRanked",				L"2������"),
	T_ReportHeaderObject(CReportScheme::ERH2DaysRise,				L"2���Ƿ�(%)",		L"2DaysRise",				L"2���Ƿ�(%)"),
	T_ReportHeaderObject(CReportScheme::ERH3DaysMasukuraProportion,	L"3������ռ��(%)",	L"3DaysMasukuraProportion",	L"3������ռ��(%)"),
	T_ReportHeaderObject(CReportScheme::ERH3DaysRanked,				L"3������",			L"3DaysRanked",				L"3������"),
	T_ReportHeaderObject(CReportScheme::ERH3DaysRise,				L"3���Ƿ�(%)",		L"3DaysRise",				L"3���Ƿ�(%)"),
	T_ReportHeaderObject(CReportScheme::ERH5DaysMasukuraProportion,	L"5������ռ��(%)",	L"5DaysMasukuraProportion",	L"5������ռ��(%)"),
	T_ReportHeaderObject(CReportScheme::ERH5DaysRanked,				L"5������",			L"5DaysRanked",				L"5������"),
	T_ReportHeaderObject(CReportScheme::ERH5DaysRise,				L"5���Ƿ�(%)",		L"5DaysRise",				L"5���Ƿ�(%)"),
	T_ReportHeaderObject(CReportScheme::ERH10DaysMasukuraProportion,L"10������ռ��(%)",	L"10DaysMasukuraProportion",L"10������ռ��(%)"),
	T_ReportHeaderObject(CReportScheme::ERH10DaysRanked,			L"10������",		L"10DaysRanked",			L"10������"),
	T_ReportHeaderObject(CReportScheme::ERH10DaysRise,				L"10���Ƿ�(%)",		L"10DaysRise",				L"10���Ƿ�(%)"),
	T_ReportHeaderObject(CReportScheme::ERHMarketMargin,			L"�о���",			L"MarketMargin",			L"�о���"),
	T_ReportHeaderObject(CReportScheme::ERHNetProfit,				L"������",			L"NetProfit",				L"������"),
	T_ReportHeaderObject(CReportScheme::ERHTotalProfit,				L"�����ܶ�",		L"TotalProfit",				L"�����ܶ�"),
	T_ReportHeaderObject(CReportScheme::ERHassetsRate,				L"���ʲ�������",	L"assetsRate",				L"���ʲ�������"),
	T_ReportHeaderObject(CReportScheme::ERHNetProfitGrowthRate,		L"������������",	L"NetProfitGrowthRate",		L"������������"),
	T_ReportHeaderObject(CReportScheme::ERHNetAmountofthemain,		L"��������",		L"NetAmountofthemain",		L"��������"),
};


const int KReportHeaderObjectCount = sizeof(s_astReportHeaderObjects) / sizeof(T_ReportHeaderObject);

//////////////////////////////////////////////////////////////////////////
// �����б�, �ñ���E_ReportTypeһһ��Ӧ
const T_ReportTypeObject s_astReportTypeObjects []=
{
	T_ReportTypeObject(ERTFuturesCn,				L"�����ڻ�",			L"ChinaFutures"),				
	T_ReportTypeObject(ERTStockCn,				L"����֤ȯ",			L"ChinaStock"),				
	T_ReportTypeObject(ERTMony,					L"������",			L"ForeigneExchange"),				
	T_ReportTypeObject(ERTExp,					L"ָ����Ŀ",			L"ShareIndex"),
	T_ReportTypeObject(ERTStockHk,				L"�۹���Ŀ",			L"HongKongStock"),
	T_ReportTypeObject(ERTFuturesForeign,		L"�����ڻ�",			L"FuturesForeign"),	
	T_ReportTypeObject(ERTWarrantCn,				L"����Ȩ֤",			L"WarrantCn"),
	T_ReportTypeObject(ERTWarrantHk,				L"���Ȩ֤",			L"WarrantHk"),
	T_ReportTypeObject(ERTFutureRelaMonth,		L"������ڻ�",			L"FutureRelaMonth"),
	T_ReportTypeObject(ERTFutureSpot,			L"�ֻ�",			    L"FutureSpot"),
	T_ReportTypeObject(ERTExpForeign,			L"����ָ��",			L"ExpForeign"),
	T_ReportTypeObject(ERTFutureGold,			L"�ƽ��ڻ�",			L"FutureGold"),
	T_ReportTypeObject(ERTCustom,				L"�Զ���",				L"Custom"),
	T_ReportTypeObject(ERTFinance,				L"��������",			L"Finance"),
	T_ReportTypeObject(ERTNone,					L"��",					L"None"),
	T_ReportTypeObject(ERTSelectStock,			L"����ѡ��",			L"SelectStock"),
	T_ReportTypeObject(ERTMerchSort,			L"����ǿ��",			L"HotStock"),
	T_ReportTypeObject(ERTCapitalFlow,			L"�ʽ�����",			L"CapitalFlow"),			 
	T_ReportTypeObject(ERTMainMasukura,			L"��������",			L"ERTMainMasukura"),          
};

const int KReportTypeObjectCount = sizeof(s_astReportTypeObjects) / sizeof(T_ReportTypeObject);  //43


typedef map<CReportScheme::E_ReportHeader, int>	ReportHeaderWidthMap;


static ReportHeaderWidthMap		s_mapReportHeaderWidthDefault;		// Ĭ�ϱ����



//static const CString KStrPath1           = L"./config/";
//static const CString KStrPath2	       = L"/grid_head.ini";

static const CString KStrIniAttriFixCol		= L"fixcol";

//////////////////////////////////////////////////////////////////////////
// ��ʼ��һЩ������е�Ĭ���п�
void	InitializeColDefaultWidth()
{
	if ( s_mapReportHeaderWidthDefault.empty() )
	{
		s_mapReportHeaderWidthDefault[ CReportScheme::ERHAllCapital ] = 130;
		s_mapReportHeaderWidthDefault[ CReportScheme::ERHCircAsset ] = 130;
		s_mapReportHeaderWidthDefault[ CReportScheme::ERHAllAsset ] = 130;
		s_mapReportHeaderWidthDefault[ CReportScheme::ERHFlowDebt ] = 130;
		s_mapReportHeaderWidthDefault[ CReportScheme::ERHBusinessProfit ] = 130;
		s_mapReportHeaderWidthDefault[ CReportScheme::ERHCircMarketValue ] = 130;
		s_mapReportHeaderWidthDefault[ CReportScheme::ERHAllMarketValue ] = 130;

		s_mapReportHeaderWidthDefault[ CReportScheme::ERHPerFund ] = 90;
		s_mapReportHeaderWidthDefault[ CReportScheme::ERHPerNoDistribute ] = 90;
		s_mapReportHeaderWidthDefault[ CReportScheme::ERHPerIncomeYear ] = 90;
		s_mapReportHeaderWidthDefault[ CReportScheme::ERHPerPureAsset ] = 90;
		s_mapReportHeaderWidthDefault[ CReportScheme::ERHChPerPureAsset ] = 100;
		s_mapReportHeaderWidthDefault[ CReportScheme::ERHDorRightRate ] = 90;
		
		s_mapReportHeaderWidthDefault[ CReportScheme::ERHCapitalFlow ] = 100;
	}
}




////////////////////////////////////////////////////////////////////////
CReportScheme::CReportScheme()
{ 
	s_Instance = this;

	InitializeColDefaultWidth();		// ��ʼ��Ĭ���п�
}

CReportScheme::~CReportScheme()
{
	s_Instance = NULL;
}

CReportScheme* CReportScheme::Instance()
{
	return s_Instance;
}



int CReportScheme::GetReportColDefaultWidth(CReportScheme::E_ReportHeader eHeaderType)
{
	ReportHeaderWidthMap::const_iterator it = s_mapReportHeaderWidthDefault.find(eHeaderType);
	if ( it != s_mapReportHeaderWidthDefault.end() )
	{
		return it->second;
	}
	return DEFUALTWIDTH;
}



void CReportScheme::Construct(CString strPrivateFolderName)
{
	m_strPrivateName = strPrivateFolderName;

	// ���
	int32 i;
	for ( i = 0; i < ERTCount; i++)
	{
		m_aReportHeadInfo[i].RemoveAll();
		m_aFixCol[i]=0;
	}
	
	// ��ֵ:
	ReadDataFromXml();

	// �Է���һ,���һ��
	bool32 bInValid = false;

	for ( i = 0 ; i < ERTCount; i++ )
	{
		E_ReportType eType = (E_ReportType)i;
		
		if ( m_aReportHeadInfo[eType].GetSize() <= 0 )
		{
			bInValid = true;
			SetHeadDataToDefault(eType, m_aReportHeadInfo[eType], m_aFixCol[eType]);
		}
	}

	if ( bInValid )
	{
		// �д�������,���±���һ��.����ԭ����.
		SaveDataToXml();
	}
}

void CReportScheme::GetReportHeadInfoList(E_ReportType eReportType,OUT CArray<T_HeadInfo,T_HeadInfo>& aHeadInfo,int32& iFixCol)
{
	if (ERTCount == eReportType )
	{
		return;
	}

	/*if (eReportType < 0)
	{
		return;
	}*/

	if (!(eReportType < sizeof(m_aFixCol)/sizeof(m_aFixCol[0])))
	{
		return;
	}

	aHeadInfo.RemoveAll();
	if (eReportType < ERTCount)
	{
		for (int32 i = 0; i < m_aReportHeadInfo[eReportType].GetSize(); i++)
		{
			T_HeadInfo HeadInfo = m_aReportHeadInfo[eReportType][i];
			aHeadInfo.Add(HeadInfo);
		}
	}

	// ��������Խ��
	if (eReportType < ERTCount)
	{
		iFixCol = m_aFixCol[eReportType];
	}
	
	
	if (aHeadInfo.GetSize()<=0)
	{
		aHeadInfo.RemoveAll();
		T_HeadInfo  HeadInfoDefault;
		for (int32 i =0;i<10;i++)
		{
			HeadInfoDefault.m_StrHeadNameCn = s_astReportHeaderObjects[i].m_StrCnName;
			//HeadInfoDefault.m_iHeadWidth	= DEFUALTWIDTH;
			HeadInfoDefault.m_iHeadWidth	= GetReportColDefaultWidth( s_astReportHeaderObjects[i].m_eReportHeader );
			HeadInfoDefault.m_eReportHeader = s_astReportHeaderObjects[i].m_eReportHeader;
			aHeadInfo.Add(HeadInfoDefault);			
		}	
		iFixCol = 0;
	}
}
CReportScheme::E_ReportHeader CReportScheme::GetReportHeaderEType(CString StrHeadCnName)
{
	StrHeadCnName = GetSpecialReportHeaderCnName(StrHeadCnName);

	bool32 bRightName = false;
	int32  iCount = 0;
	for (int32 i = 0;i<KReportHeaderObjectCount;i++)
	{
		if (StrHeadCnName == s_astReportHeaderObjects[i].m_StrCnName)
		{
			bRightName = TRUE;
			iCount     = i;
			return s_astReportHeaderObjects[iCount].m_eReportHeader;
		}		
	}
	//ASSERT(bRightName && iCount>=0 && iCount<KReportHeaderObjectCount);
	return ERHCount;	// ���طǷ�ֵ
}

CString CReportScheme::GetReportHeaderCnName(E_ReportHeader eReportHeader)
{
	ASSERT(eReportHeader < ERHCount);
	ASSERT(ERHCount == KReportHeaderObjectCount);
	if (eReportHeader >= ERHCount )
	{
		return _T("");
	}

	CString strName = s_astReportHeaderObjects[eReportHeader].m_StrCnName;
	strName = GetSpecialReportHeaderCnName(strName);
	return strName;
//	return s_astReportHeaderObjects[eReportHeader].m_StrCnName;
}

CString CReportScheme::GetReportTypeCnName(E_ReportType e_ReportType)
{
	ASSERT(e_ReportType < ERTCount );
	ASSERT(ERTCount == KReportTypeObjectCount);
	if (e_ReportType >= ERTCount )
	{
		return _T("");
	}
	return s_astReportTypeObjects[e_ReportType].m_StrCnName;
}

void CReportScheme::GetHead(E_ReportType eReportType,OUT CStringArray& aStrGridHead,OUT CStringArray& aStrAnotherGridHead)
{
	// ����Type ,��ȡ���ڵı�ͷ�����Ĳ���
	ASSERT(eReportType<ERTCount);
	ASSERT(ERTCount == KReportTypeObjectCount);
	if (ERTCount == eReportType)
	{
		return;
	}
	/*if (eReportType < 0)
	{
		return;
	}*/

	if (!(eReportType < sizeof(m_aReportHeadInfo)/sizeof(m_aReportHeadInfo[0])))
	{
		return;
	}

	int32 iSize =  m_aReportHeadInfo[eReportType].GetSize();
	aStrGridHead.RemoveAll();
	aStrAnotherGridHead.RemoveAll();

	if (ERTMony == eReportType)
	{
		for (int32 i = 0; i<KReportHeaderObjectCount ; i++)
		{
			if (ERHPriceNew == s_astReportHeaderObjects[i].m_eReportHeader)
			{
				aStrAnotherGridHead.Add(m_strNewPrice);
			}
			else if (ERHBuyPrice == s_astReportHeaderObjects[i].m_eReportHeader)
			{
				aStrAnotherGridHead.Add(m_strBuyPrice);
			}
			else if (ERHSellPrice == s_astReportHeaderObjects[i].m_eReportHeader)
			{
				aStrAnotherGridHead.Add(m_strSellPrice);
			}
			else
			{
				aStrAnotherGridHead.Add(s_astReportHeaderObjects[i].m_StrSummary);
			}
		}
		for (int32 p=0;p<iSize;p++ )
		{
			for (int32 q=0;q<KReportHeaderObjectCount;q++)
			{
				CString strHeadName = s_astReportHeaderObjects[q].m_StrCnName;
				strHeadName = HeadSpecialCnName2HeadCnName(strHeadName);
				if ( m_aReportHeadInfo[eReportType][p].m_StrHeadNameCn == strHeadName)
				{
					if (ERHPriceNew == s_astReportHeaderObjects[q].m_eReportHeader)
					{
						aStrGridHead.Add(m_strNewPrice);
					}
					else if (ERHBuyPrice == s_astReportHeaderObjects[q].m_eReportHeader)
					{
						aStrGridHead.Add(m_strBuyPrice);
					}
					else if (ERHSellPrice == s_astReportHeaderObjects[q].m_eReportHeader)
					{
						aStrGridHead.Add(m_strSellPrice);
					}
					else
					{
						aStrGridHead.Add(s_astReportHeaderObjects[q].m_StrSummary);
					}
					break;
				}
			}
		}
	}
	else
	{
		for (int32 i = 0; i<KReportHeaderObjectCount ; i++)
		{
			aStrAnotherGridHead.Add(s_astReportHeaderObjects[i].m_StrSummary);
		}
		for (int32 p=0; p<iSize; p++ )
		{
			for (int32 q=0; q<KReportHeaderObjectCount; q++)
			{
				if ( m_aReportHeadInfo[eReportType][p].m_StrHeadNameCn == s_astReportHeaderObjects[q].m_StrCnName)
				{
					aStrGridHead.Add(s_astReportHeaderObjects[q].m_StrSummary);
					break;
				}
			}
		}
	}

	for ( int32 j=0;j < aStrGridHead.GetSize();j++)
	{
		for (int32 i=0;i<aStrAnotherGridHead.GetSize();i++)
		{
			if ( aStrGridHead[j] == aStrAnotherGridHead[i] )
			{	
				aStrAnotherGridHead.RemoveAt(i);
			}
		}		
	}	
}

void CReportScheme::GetHead(IN CStringArray& aStrGridHead,OUT CStringArray& aStrAnotherGridHead,E_ReportType eReportType)
{
	// ��֪һ����ͷ����,��ȡ���Ĳ���

	ASSERT(ERTCount == KReportTypeObjectCount);

	aStrAnotherGridHead.RemoveAll();

	if (ERTMony == eReportType)
	{
		for (int32 i = 0; i<KReportHeaderObjectCount ; i++)
		{
			if (ERHPriceNew==i)
			{
				aStrAnotherGridHead.Add(m_strNewPrice);
			}
			else if (ERHBuyPrice==i)
			{
				aStrAnotherGridHead.Add(m_strBuyPrice);
			}
			else if (ERHSellPrice==i)
			{
				aStrAnotherGridHead.Add(m_strSellPrice);
			}
			else
			{
				aStrAnotherGridHead.Add(s_astReportHeaderObjects[i].m_StrSummary);
			}
		}
		for ( int32 j=0;j < aStrGridHead.GetSize();j++)
		{
			for (int32 i=0;i<aStrAnotherGridHead.GetSize();i++)
			{			
				if ( aStrGridHead[j] == aStrAnotherGridHead[i] )
				{	
					aStrAnotherGridHead.RemoveAt(i);
				}
			}		
		}	
	}
	else
	{
		for (int32 i = 0; i<KReportHeaderObjectCount ; i++)
		{
			aStrAnotherGridHead.Add(s_astReportHeaderObjects[i].m_StrSummary);
		}
		for ( int32 j=0;j < aStrGridHead.GetSize();j++)
		{
			for (int32 i=0;i<aStrAnotherGridHead.GetSize();i++)
			{
				if ( aStrGridHead[j] == aStrAnotherGridHead[i] )
				{	
					aStrAnotherGridHead.RemoveAt(i);
				}
			}		
		}	
	}
}

void CReportScheme::SaveColumnWidth(E_ReportType eReportType,int32 iColumn,int32 iWidth)
{

	ASSERT(eReportType<ERTCount);
	ASSERT(ERTCount == KReportTypeObjectCount);
	/*if (eReportType < 0)
	{
		return;
	}*/

	if (!(eReportType < sizeof(m_aReportHeadInfo)/sizeof(m_aReportHeadInfo[0])))
	{
		return;
	}
	ASSERT(iColumn>=0 && iColumn<m_aReportHeadInfo[eReportType].GetSize());

	if (iColumn>=0 &&  iColumn<m_aReportHeadInfo[eReportType].GetSize())
	{
		return;
	}
	if (iWidth<=0||iWidth>=MAXWIDTH)
	{
		iWidth = MAXWIDTH;
	}
	

	// ����ȫ�������ֵ
	int32& iWidthTest = m_aReportHeadInfo[eReportType][iColumn].m_iHeadWidth;
	iWidthTest = iWidth;	
}

void CReportScheme::DoHeadInfoString(CString StrHeadInfo,OUT CString& StrHeadName,OUT int32& iWidth)
{
	ASSERT(0 != StrHeadInfo.GetLength());
	StrHeadInfo.TrimLeft();
	StrHeadInfo.TrimRight();
	
	int32 iStart = StrHeadInfo.Find(_T("<"));
	StrHeadName  = StrHeadInfo.Left(iStart);

	int32 iEnd	 = StrHeadInfo.Find(_T(">"));
	CString StrWidth = StrHeadInfo.Mid(iStart+1,iEnd-iStart-1);
	iWidth = _ttoi(StrWidth);
	
	// �����Ȳ���������Χ,����ΪĬ��
	if (iWidth<=0 || iWidth>=MAXWIDTH)
	{
		iWidth = MAXWIDTH;
	}
	// ������Ʋ���������Χ,����ΪĬ��
	bool32 bRightName = false;
	for (int32 i = 0;i<KReportHeaderObjectCount;i++)
	{
		if (StrHeadName == s_astReportHeaderObjects[i].m_StrCnName)
		{
			bRightName = TRUE;
		}		
	}
	if (!bRightName)
	{
		StrHeadName = s_astReportHeaderObjects[0].m_StrCnName;
	}
}
CString CReportScheme::HeadCnName2SummaryName(CString StrCnName)
{
	if ((StrCnName==m_strBuyPrice)||(StrCnName==m_strSellPrice)||(StrCnName==m_strNewPrice))
	{
		return StrCnName;
	}

	for (int32 i = 0; i<KReportHeaderObjectCount ; i++)
	{
		if (StrCnName == s_astReportHeaderObjects[i].m_StrCnName)
		{
			return s_astReportHeaderObjects[i].m_StrSummary;
		}
	}
	return L"ERROR";
}
CString CReportScheme::HeadSummaryName2CnName(CString StrSummaryName)
{
	if ((StrSummaryName==m_strBuyPrice)||(StrSummaryName==m_strSellPrice)||(StrSummaryName==m_strNewPrice))
	{
		return StrSummaryName;
	}

	for (int32 i = 0; i<KReportHeaderObjectCount ; i++)
	{
		if (StrSummaryName == s_astReportHeaderObjects[i].m_StrSummary)
		{
			return s_astReportHeaderObjects[i].m_StrCnName;
		}
	}
	return L"ERROR";
}

CString	CReportScheme::HeadSpecialCnName2HeadCnName(CString strName)
{
	CString strRet = strName;
	if (strName == m_strBuyPrice)
	{
		strRet = L"���";
	}
	else if (strName == m_strSellPrice)
	{
		strRet = L"����";
	}
	else if (strName == m_strNewPrice)
	{
		strRet = L"����";
	}
	return strRet;
}

CString	CReportScheme::GetSpecialReportHeaderCnName(CString strName)
{
	CString strRetName = strName;

	if (strRetName == m_strBuyPrice)
	{
		strRetName = L"���";
	}
	else if (strRetName == m_strSellPrice)
	{
		strRetName = L"����";
	}
	else if (strRetName == m_strNewPrice)
	{
		strRetName = L"����";
	}
	return strRetName;
}

CString CReportScheme::GetPath()
{
/*	
	CString StrPath,StrUserName;
				
	ASSERT(NULL != m_pDocument);
	StrUserName = m_pDocument->m_StrUserName;

	StrPath = KStrPath1 +  StrUserName + KStrPath2;	
	return StrPath;
*/

	CString StrPath;
	StrPath = CPathFactory::GetGridHeadFilePath(m_strPrivateName);
	return StrPath;
}

CString CReportScheme::GetFixColAttriName()
{
	return KStrIniAttriFixCol;
}

void CReportScheme::SetHeadDataToDefault(E_ReportType eReportType,CArray<T_HeadInfo,T_HeadInfo>& aHeadInfo,int32& iFixCol)
{
	// ��ĳ���ͷ����������ΪĬ������:
	if (eReportType >= ERTCount )
	{
		return;
	}
	
	// ���
	aHeadInfo.RemoveAll();
	iFixCol	= 0;
	
	// ������:
	int32 iSize = 0;
	
	if (eReportType == ERTFuturesCn || eReportType == ERTFutureRelaMonth) 
	{
		// �����ڻ� 
		iFixCol = 3;
		if ( eReportType == ERTFutureRelaMonth )			
		{
			iFixCol = 2;
		}
		//
		T_HeadInfo aDefaul[] = 
		{
			T_HeadInfo(L"����",		81),
			T_HeadInfo(L"����",		70),
			T_HeadInfo(L"�ǵ�",		70),
			T_HeadInfo(L"����",	70),
			T_HeadInfo(L"����",		70),
			T_HeadInfo(L"���",		70),
			T_HeadInfo(L"����",		70),
			T_HeadInfo(L"����",		70),
			T_HeadInfo(L"����",		70),
			T_HeadInfo(L"����",		70),
			T_HeadInfo(L"���",		70),
			T_HeadInfo(L"���",		70),
			T_HeadInfo(L"����",		70),
			T_HeadInfo(L"�ֲ���",	70),
			T_HeadInfo(L"�ֲ�",		70),
			T_HeadInfo(L"���",		70),
			T_HeadInfo(L"����",		70),
			T_HeadInfo(L"����",		70),
			T_HeadInfo(L"���%",	70),
			T_HeadInfo(L"�Ƿ�%",	70),
			T_HeadInfo(L"�ܽ��",		70),
			T_HeadInfo(L"��",		35),
			T_HeadInfo(L"ʱ��",		70),
		};
		
		iSize = sizeof(aDefaul)/sizeof(T_HeadInfo);
		
		for ( int32 i = 0 ; i < iSize; i++ )
		{
			T_HeadInfo HeadInfoDefault = aDefaul[i];
			aHeadInfo.Add(HeadInfoDefault);		
		}			
	}
	else if( eReportType == ERTStockCn || eReportType == ERTCustom)
	{
		// ����֤ȯ
		iFixCol = 3;
		//
		T_HeadInfo aDefaul[] = 
		{
			T_HeadInfo(L"����",		70),
			T_HeadInfo(L"����",		79),
			T_HeadInfo(L"�Ƿ�%",	66),

			T_HeadInfo(L"����",	70),
			T_HeadInfo(L"����",	70),
			T_HeadInfo(L"�ʽ�����",	100),

			T_HeadInfo(L"����",		64),
			T_HeadInfo(L"�ǵ�",		63),
			T_HeadInfo(L"���",		72),
			T_HeadInfo(L"����",		73),
			T_HeadInfo(L"����",	70),
			T_HeadInfo(L"����",		62),
			T_HeadInfo(L"����",		62),
			T_HeadInfo(L"����",		62),
			T_HeadInfo(L"���",		62),
			T_HeadInfo(L"���",		62),
			T_HeadInfo(L"�ܽ��",		65),
			T_HeadInfo(L"���%",	64),
			T_HeadInfo(L"����",		62),
			T_HeadInfo(L"����",		62),
			T_HeadInfo(L"��",		35),
			T_HeadInfo(L"ʱ��",		70),
		};
		
		iSize = sizeof(aDefaul)/sizeof(T_HeadInfo);
		
		for ( int32 i = 0 ; i < iSize; i++ )
		{
			T_HeadInfo HeadInfoDefault = aDefaul[i];
			aHeadInfo.Add(HeadInfoDefault);		
		}
	}
	else if ( eReportType == ERTMony)
	{
		// ���
		iFixCol = 2;
		//
		T_HeadInfo aDefaul[] = 
		{
			T_HeadInfo(L"����",		70),
			T_HeadInfo(m_strNewPrice,		70),
			T_HeadInfo(L"�ǵ�",		70),
			T_HeadInfo(m_strBuyPrice,		70),
			T_HeadInfo(m_strSellPrice,		70),
			T_HeadInfo(L"����",		70),
			T_HeadInfo(L"���",		70),
			T_HeadInfo(L"���",		70),
			T_HeadInfo(L"�Ƿ�%",	70),
			T_HeadInfo(L"���%",	70),
			T_HeadInfo(L"����",		70),
			T_HeadInfo(L"����",		70),
			T_HeadInfo(L"��",		35),
			T_HeadInfo(L"ʱ��",		70),
		};
		
		iSize = sizeof(aDefaul)/sizeof(T_HeadInfo);
		
		for ( int32 i = 0 ; i < iSize; i++ )
		{
			T_HeadInfo HeadInfoDefault = aDefaul[i];
			aHeadInfo.Add(HeadInfoDefault);		
		}
	}
	else if( eReportType == ERTExp)
	{
		// ָ��
		iFixCol = 3;
		//
		T_HeadInfo aDefaul[] = 
		{
			T_HeadInfo(L"����",		70),
			T_HeadInfo(L"����",		70),
			T_HeadInfo(L"�Ƿ�%",	70),
			T_HeadInfo(L"����",		70),
			T_HeadInfo(L"�ǵ�",		70),
			T_HeadInfo(L"����",	70),
			T_HeadInfo(L"����",		70),
			T_HeadInfo(L"���",		70),
			T_HeadInfo(L"���",		70),
			T_HeadInfo(L"����",		70),
			T_HeadInfo(L"���%",	70),
			T_HeadInfo(L"�ܽ��",		70),
			T_HeadInfo(L"��",		35),
			T_HeadInfo(L"ʱ��",		70),
		};
		
		iSize = sizeof(aDefaul)/sizeof(T_HeadInfo);
		
		for ( int32 i = 0 ; i < iSize; i++ )
		{
			T_HeadInfo HeadInfoDefault = aDefaul[i];
			aHeadInfo.Add(HeadInfoDefault);		
		}	
	}
	else if ( eReportType == ERTStockHk)
	{
		// �۹�
		iFixCol = 2;
		//
		T_HeadInfo aDefaul[] = 
		{			
			T_HeadInfo(L"����",		70),
			T_HeadInfo(L"����",		70),
			T_HeadInfo(L"�Ƿ�%",	70),
			T_HeadInfo(L"����",		70),
			T_HeadInfo(L"�ǵ�",		70),
			T_HeadInfo(L"���",		70),
			T_HeadInfo(L"����",		70),
			T_HeadInfo(L"����",		70),
			T_HeadInfo(L"����",		70),
			T_HeadInfo(L"����",	70),
			T_HeadInfo(L"����",		70),
			T_HeadInfo(L"����",		70),
			T_HeadInfo(L"����",		70),
			T_HeadInfo(L"���",		70),
			T_HeadInfo(L"���",		70),
			T_HeadInfo(L"�ܽ��",		70),
			T_HeadInfo(L"ʱ��",		70),
			T_HeadInfo(L"��",		35),
		};
		
		iSize = sizeof(aDefaul)/sizeof(T_HeadInfo);
		
		for ( int32 i = 0 ; i < iSize; i++ )
		{
			T_HeadInfo HeadInfoDefault = aDefaul[i];
			aHeadInfo.Add(HeadInfoDefault);						
		}
	}
	else if (eReportType == ERTFuturesForeign)
	{
		// �����ڻ�
		iFixCol = 2;
		//
		T_HeadInfo aDefaul[] = 
		{	
			T_HeadInfo(L"����",		70),
			T_HeadInfo(L"����",		70),
			T_HeadInfo(L"�ǵ�",		70),
			T_HeadInfo(L"����",	70),
			T_HeadInfo(L"����",		70),
			T_HeadInfo(L"���",		70),
			T_HeadInfo(L"���",		70),
			T_HeadInfo(L"���",		70),
			T_HeadInfo(L"����",		70),
			T_HeadInfo(L"����",		70),
			T_HeadInfo(L"�ֲ���",	70),
			T_HeadInfo(L"�ֲ�",		70),
			T_HeadInfo(L"�Ƿ�%",	70),
			T_HeadInfo(L"���%",	70),
			T_HeadInfo(L"��",		35),
			T_HeadInfo(L"����",		70),
			T_HeadInfo(L"ʱ��",		70),
		};
		
		iSize = sizeof(aDefaul)/sizeof(T_HeadInfo);
		
		for ( int32 i = 0 ; i < iSize; i++ )
		{
			T_HeadInfo HeadInfoDefault = aDefaul[i];
			aHeadInfo.Add(HeadInfoDefault);		
		}
	}
	else if (eReportType == ERTWarrantCn)
	{
		// Ȩ֤
		iFixCol = 2;
		//
		T_HeadInfo aDefaul[] = 
		{	
			T_HeadInfo(L"����",		70),
			T_HeadInfo(L"����",		70),
			T_HeadInfo(L"�Ƿ�%",	70),
			T_HeadInfo(L"����",		70),
			T_HeadInfo(L"�ǵ�",		70),
			T_HeadInfo(L"����",	70),
			T_HeadInfo(L"����",		70),
			T_HeadInfo(L"���",		70),
			T_HeadInfo(L"����",		70),
			T_HeadInfo(L"����",		70),
			T_HeadInfo(L"����",		70),
			T_HeadInfo(L"����",		70),
			T_HeadInfo(L"����",		70),
			T_HeadInfo(L"���",		70),
			T_HeadInfo(L"���",		70),
			T_HeadInfo(L"�ܽ��",		70),
			T_HeadInfo(L"ʱ��",		70),
			T_HeadInfo(L"��",		35),
		};
		
		iSize = sizeof(aDefaul)/sizeof(T_HeadInfo);
		
		for ( int32 i = 0 ; i < iSize; i++ )
		{
			T_HeadInfo HeadInfoDefault = aDefaul[i];
			aHeadInfo.Add(HeadInfoDefault);		
		}				
	}
	else if ( eReportType == ERTWarrantHk ) 
	{
		iFixCol = 2;
		//
		T_HeadInfo aDefaul[] = 
		{	
			T_HeadInfo(L"����",			70),
			T_HeadInfo(L"����",			70),
			T_HeadInfo(L"�Ϲ��Ϲ�",		70),
			T_HeadInfo(L"��Ȩ��",		70),
			T_HeadInfo(L"��Ȩ��",		70),
			T_HeadInfo(L"������",		70),
			T_HeadInfo(L"������",		70),
			T_HeadInfo(L"���ۼ�",		70),
			T_HeadInfo(L"���",			70),
			T_HeadInfo(L"ŷʽ��ʽ",		70),
			T_HeadInfo(L"��ͨ���",		70),
			T_HeadInfo(L"������",		70),
			T_HeadInfo(L"Ȩ֤����",		70),
			T_HeadInfo(L"���ڼ���",		70),
			T_HeadInfo(L"���ڼ�ֵ",		70),
			T_HeadInfo(L"��ʷ����",		70),
			T_HeadInfo(L"���첨��",		70),
			T_HeadInfo(L"�Գ�ֵ",		70),
			T_HeadInfo(L"GAMMA",		70),
			T_HeadInfo(L"�ܸ˱���",		70),
			T_HeadInfo(L"ʵ�ʸܸ�",		70),
			T_HeadInfo(L"���ɱ���",		70),
			T_HeadInfo(L"�ֻ���",		70),
			T_HeadInfo(L"�ֻ�����",		70),
			T_HeadInfo(L"����",			70),
			T_HeadInfo(L"ƽ��",			70),
			T_HeadInfo(L"��",			35),
		};
		
		iSize = sizeof(aDefaul)/sizeof(T_HeadInfo);
		
		for ( int32 i = 0 ; i < iSize; i++ )
		{
			T_HeadInfo HeadInfoDefault = aDefaul[i];
			aHeadInfo.Add(HeadInfoDefault);		
		}
	}
	else if ( eReportType == ERTFutureSpot )
	{
		iFixCol = 2;
		//
		T_HeadInfo aDefaul[] = 
		{	
			T_HeadInfo(L"����",			70),
			T_HeadInfo(L"����",			70),
			T_HeadInfo(L"���",			70),
			T_HeadInfo(L"����",			70),
			T_HeadInfo(L"����",			70),
			T_HeadInfo(L"���",			70),
			T_HeadInfo(L"���",			70),
			T_HeadInfo(L"����",			70),
			T_HeadInfo(L"����",			70),	
			T_HeadInfo(L"�ֲ���",		70),
			T_HeadInfo(L"����",			70),	
			T_HeadInfo(L"�ܽ��",			70),
			T_HeadInfo(L"����",		70),
			T_HeadInfo(L"�ǵ�",			70),
			T_HeadInfo(L"�Ƿ�%",		70),
			
			T_HeadInfo(L"��ټ�",		70),
			T_HeadInfo(L"���ټ�",		70),
			T_HeadInfo(L"�����",		70),
			T_HeadInfo(L"������",		70),

			T_HeadInfo(L"��ڼ�",		70),
			T_HeadInfo(L"���ڼ�",		70),
			T_HeadInfo(L"�����",		70),
			T_HeadInfo(L"������",		70),

			T_HeadInfo(L"��ۼ�",		70),
			T_HeadInfo(L"���ۼ�",		70),
			T_HeadInfo(L"�����",		70),
			T_HeadInfo(L"������",		70),

			T_HeadInfo(L"��ܼ�",		70),
			T_HeadInfo(L"���ܼ�",		70),
			T_HeadInfo(L"�����",		70),
			T_HeadInfo(L"������",		70),

			T_HeadInfo(L"��ݼ�",		70),
			T_HeadInfo(L"���ݼ�",		70),
			T_HeadInfo(L"�����",		70),
			T_HeadInfo(L"������",		70),
		};
		
		iSize = sizeof(aDefaul)/sizeof(T_HeadInfo);
		
		for ( int32 i = 0 ; i < iSize; i++ )
		{
			T_HeadInfo HeadInfoDefault = aDefaul[i];
			aHeadInfo.Add(HeadInfoDefault);		
		}
	}
	else if ( eReportType == ERTExpForeign )
	{
		iFixCol = 2;
		//
		T_HeadInfo aDefaul[] = 
		{	
			T_HeadInfo(L"����",			70),
			T_HeadInfo(L"�Ƿ�%",		70),
			T_HeadInfo(L"����",			70),
			T_HeadInfo(L"�ǵ�",			70),
			T_HeadInfo(L"����",		70),
			T_HeadInfo(L"����",			70),
			T_HeadInfo(L"���",			70),
			T_HeadInfo(L"���",			70),
			T_HeadInfo(L"����",			70),
			T_HeadInfo(L"����",			70),
			T_HeadInfo(L"��",			35),
			T_HeadInfo(L"ʱ��",			70),
		};
		
		iSize = sizeof(aDefaul)/sizeof(T_HeadInfo);
		
		for ( int32 i = 0 ; i < iSize; i++ )
		{
			T_HeadInfo HeadInfoDefault = aDefaul[i];
			aHeadInfo.Add(HeadInfoDefault);						
		}
	}
	else if ( eReportType == ERTFutureGold )
	{
		iFixCol = 3;
		//
		T_HeadInfo aDefaul[] = 
		{	
			T_HeadInfo(L"����",			70),
			T_HeadInfo(L"����",			70),
			T_HeadInfo(L"����",			70),
			T_HeadInfo(L"���",			70),
			T_HeadInfo(L"����",			70),
			T_HeadInfo(L"����",			70),
			T_HeadInfo(L"����",			70),
			T_HeadInfo(L"����",		70),
			T_HeadInfo(L"�ǵ�",			70),
			T_HeadInfo(L"�Ƿ�%",		70),
			T_HeadInfo(L"����",			70),
			T_HeadInfo(L"���",			70),
			T_HeadInfo(L"���",			70),
			T_HeadInfo(L"����",			70),
			T_HeadInfo(L"����",			70),
			T_HeadInfo(L"��",			70),
			T_HeadInfo(L"����",			70),
			T_HeadInfo(L"ʱ��",			70),
		};
		
		iSize = sizeof(aDefaul)/sizeof(T_HeadInfo);
		
		for ( int32 i = 0 ; i < iSize; i++ )
		{
			T_HeadInfo HeadInfoDefault = aDefaul[i];
			aHeadInfo.Add(HeadInfoDefault);		
		}
	}
	else if ( eReportType == ERTNone )		
	{
		//��ЩӦ�ò���ʾ��
		iFixCol = 1;
		
		//
		T_HeadInfo aDefaul[] = 
		{	
			T_HeadInfo(L"��",		70),
		};
		
		iSize = sizeof(aDefaul) / sizeof(T_HeadInfo);
		
		for ( int32 i = 0 ; i < iSize; i++ )
		{
			T_HeadInfo HeadInfoDefault = aDefaul[i];
			aHeadInfo.Add(HeadInfoDefault);		
		}
	}
	else if( eReportType == ERTSelectStock )
	{
		// ����ѡ��(����֤ȯ)
		iFixCol = 3;
		//
		T_HeadInfo aDefaul[] = 
		{
			T_HeadInfo(L"����",		70),
			T_HeadInfo(L"����",		79),
			T_HeadInfo(L"�Ƿ�%",	66),
			T_HeadInfo(L"����",		66),
			T_HeadInfo(L"����",		66),
			T_HeadInfo(L"�ʽ�����",	70),
			T_HeadInfo(L"����",		64),
			T_HeadInfo(L"�ǵ�",		63),
			T_HeadInfo(L"���",		72),
			T_HeadInfo(L"����",		73),
			T_HeadInfo(L"����",	70),
			T_HeadInfo(L"����",		62),
			T_HeadInfo(L"����",		62),
			T_HeadInfo(L"����",		62),
			T_HeadInfo(L"���",		62),
			T_HeadInfo(L"���",		62),
			T_HeadInfo(L"�ܽ��",		65),
			T_HeadInfo(L"���%",	64),
			T_HeadInfo(L"����",		62),
			T_HeadInfo(L"����",		62),
			T_HeadInfo(L"��",		35),
			T_HeadInfo(L"ʱ��",		70),
		};
		
		iSize = sizeof(aDefaul)/sizeof(T_HeadInfo);
		
		for ( int32 i = 0 ; i < iSize; i++ )
		{
			T_HeadInfo HeadInfoDefault = aDefaul[i];
			aHeadInfo.Add(HeadInfoDefault);		
		}
	}
	else if( eReportType == ERTFinance )
	{
		// ��������(����֤ȯ)
		iFixCol = 3;
		//
		T_HeadInfo aDefaul[] = 
		{
			T_HeadInfo(L"����",			  70),
			T_HeadInfo(L"����",		      79),
			T_HeadInfo(L"�Ƿ�%",	      66),
			T_HeadInfo(L"�ܹɱ�(��)"    , 130),
			T_HeadInfo(L"���ʲ�(��)"    , 130),
			T_HeadInfo(L"����ֵ(��)"    , 130),
			T_HeadInfo(L"��ͨ��ֵ(��)"  , 130),
			T_HeadInfo(L"������ծ(��)"  , 130),
			T_HeadInfo(L"ÿ�ɹ�����"    , 92),
			T_HeadInfo(L"Ӫҵ����(��)"  , 130),
			T_HeadInfo(L"ÿ��δ����"    , 92),
			T_HeadInfo(L"ÿ������"      , 90),
			T_HeadInfo(L"ÿ�ɾ��ʲ�"    , 92),
			T_HeadInfo(L"����ÿ�ɾ��ʲ�", 130),
			T_HeadInfo(L"�ɶ�Ȩ���"    , 92),
		};

		iSize = sizeof(aDefaul)/sizeof(T_HeadInfo);

		for ( int32 i = 0 ; i < iSize; i++ )
		{
			T_HeadInfo HeadInfoDefault = aDefaul[i];
			aHeadInfo.Add(HeadInfoDefault);		
		}
	}
	else if( eReportType == ERTMerchSort )
	{
		// ����ǿ��(����֤ȯ)
		iFixCol = 3;
		//
		T_HeadInfo aDefaul[] = 
		{
			T_HeadInfo(L"����",		70),
			T_HeadInfo(L"����",		79),
			T_HeadInfo(L"�Ƿ�%",	66),
			T_HeadInfo(L"����",		66),
			T_HeadInfo(L"����",		66),
			T_HeadInfo(L"�ʽ�����",	70),
			T_HeadInfo(L"����",		64),
			T_HeadInfo(L"�ǵ�",		63),
			T_HeadInfo(L"���",		72),
			T_HeadInfo(L"����",		73),
			T_HeadInfo(L"����",	70),
			T_HeadInfo(L"����",		62),
			T_HeadInfo(L"����",		62),
			T_HeadInfo(L"����",		62),
			T_HeadInfo(L"���",		62),
			T_HeadInfo(L"���",		62),
			T_HeadInfo(L"�ܽ��",		65),
			T_HeadInfo(L"���%",	64),
			T_HeadInfo(L"����",		62),
			T_HeadInfo(L"����",		62),
			T_HeadInfo(L"��",		35),
			T_HeadInfo(L"ʱ��",		70),
		};

		iSize = sizeof(aDefaul)/sizeof(T_HeadInfo);

		for ( int32 i = 0 ; i < iSize; i++ )
		{
			T_HeadInfo HeadInfoDefault = aDefaul[i];
			aHeadInfo.Add(HeadInfoDefault);		
		}
	}
}

void CReportScheme::ChangeHeadInfoData(E_ReportType eReportType,CArray<T_HeadInfo,T_HeadInfo>& aHeadInfo,int32& iFixCol)
{
	if (eReportType >= ERTCount )
	{
		return;
	}

	m_aReportHeadInfo[eReportType].RemoveAll();
	m_aReportHeadInfo[eReportType].Copy(aHeadInfo);

	m_aFixCol[eReportType] = iFixCol;
}

void CReportScheme::SetBuySellPriceInfo(CString strBuyPrice, CString strSellPrice, CString strNewPrice)
{
	m_strBuyPrice = strBuyPrice;
	m_strSellPrice = strSellPrice;
	m_strNewPrice = strNewPrice;
}

bool32 CReportScheme::ReadDataFromXml()
{
	// ��˽��Ŀ¼�µ� grid_head.xml �ж�ȡ�û��Զ�������:


	CString StrPath = GetPath();

	std::string StrPathA ;
	Unicode2MultiChar(CP_ACP, StrPath, StrPathA);
	TiXmlDocument MyDoc = TiXmlDocument(StrPathA.c_str());
	if ( !MyDoc.LoadFile() )
	{	
		// xl 1103 ��������Ŀ¼�¿�������Ĭ�ϱ�ͷ��xml����
		if ( !CPathFactory::GetExistConfigFileName(StrPath, CPathFactory::GetGridHeadFileName()) )
		{
			return false;
		}
		Unicode2MultiChar(CP_ACP, StrPath, StrPathA);		
		if ( !MyDoc.LoadFile(StrPathA.c_str()) )
		{
			return false;
		}
		// ����������ˣ����������
	}
	
	// XMLDATA
	TiXmlElement* pRootElement = MyDoc.FirstChildElement();
	
	if ( NULL == pRootElement )
	{
		return false;
	}

	TiXmlElement* pHeadInfoElment = pRootElement->FirstChildElement();

	while(pHeadInfoElment)
	{
		if ( NULL != pHeadInfoElment->Value() && 0 == strcmp(pHeadInfoElment->Value(), KStrElementNameHeadInfo) )
		{
			// �� <HeadInfo Tpye = "1" FixCol = "1"> ����ڵ�

			// Type: ��ͷ����
			const char* StrType		= pHeadInfoElment->Attribute(KStrElementAttriHeadType);
			
			// FixCol: ��ͷ�̶������
			const char* StrFixCol	= pHeadInfoElment->Attribute(KStrElementAttriFixCol);
			
			if ( NULL == StrType )
			{
				pHeadInfoElment = pHeadInfoElment->NextSiblingElement();
				continue;
			}

			int32 iType = atoi(StrType);

			if ( iType < 0 || iType >= (int32)ERTCount )
			{
				pHeadInfoElment = pHeadInfoElment->NextSiblingElement();
				continue;
			}

			// 
			E_ReportType eReportType = (E_ReportType)iType;
			int32		 iFixCol	 = 0;	
			
			if ( NULL != StrFixCol )
			{
				iFixCol = atoi(StrFixCol);
				m_aFixCol[iType] = iFixCol;
			}
			
			// ��ȡ��������µı�ͷ����:
			// ���������:
			m_aReportHeadInfo[eReportType].RemoveAll();				

			TiXmlElement* pHeadItemElement = pHeadInfoElment->FirstChildElement();
			
			while(pHeadItemElement)
			{
				if ( NULL != pHeadItemElement->Value() && 0 == strcmp(pHeadItemElement->Value(), KStrElementNameHeadItem) )
				{
					// �� <HeadItem Name = "��" Width = "70"> ����ڵ�
					const char* StrName  = pHeadItemElement->Attribute(KStrElementAttriHeadName);
					const char* StrWidth = pHeadItemElement->Attribute(KStrElementAttriHeadWidth);
					const char* StrHeaderColType = pHeadItemElement->Attribute(KStrElementAttriHeadColumnType);

					if ( NULL == StrName )
					{
						// �����Ǵ�������,ֻ�к���������
						pHeadItemElement = pHeadItemElement->NextSiblingElement();
						continue;
					}
					
					T_HeadInfo HeadInfo;
					
					// �ַ�������,���ַ�תΪ���ַ�:
					TCHAR wStrName[1024] = {'0'};
					
					MultiCharCoding2Unicode(EMCCUtf8, StrName, strlen(StrName), wStrName, sizeof(wStrName)/sizeof(TCHAR));
					
					HeadInfo.m_StrHeadNameCn = wStrName;
					//HeadInfo.m_iHeadWidth	 = 70;

					CReportScheme::E_ReportHeader eHeader = CReportScheme::Instance()->GetReportHeaderEType(wStrName);
					if (eHeader >= ERHCount )
					{
						// û������ֶ�, ����һ��xml type��
						E_ReportHeader eHeaderXml = ERHCount;
						if ( NULL != StrHeaderColType )
						{
							eHeaderXml = (E_ReportHeader)atoi(StrHeaderColType);
						}
						if (eHeaderXml<ERHCount )
						{
							HeadInfo.m_StrHeadNameCn = CReportScheme::Instance()->GetReportHeaderCnName(eHeaderXml);
						}
						else
						{
							HeadInfo.m_StrHeadNameCn.Empty();
						}
						if ( !HeadInfo.m_StrHeadNameCn.IsEmpty() )
						{
							eHeader = eHeaderXml;
						}
						else
						{
							// ������
							pHeadItemElement = pHeadItemElement->NextSiblingElement();
							continue;
						}
					}
					HeadInfo.m_eReportHeader = eHeader;
					HeadInfo.m_iHeadWidth    = CReportScheme::GetReportColDefaultWidth(eHeader);

					if ( NULL != StrWidth )
					{
						HeadInfo.m_iHeadWidth = atoi(StrWidth);
					}

					if (ERTMony==eReportType)
					{
						if (L"���" == HeadInfo.m_StrHeadNameCn)
						{
							HeadInfo.m_StrHeadNameCn = m_strBuyPrice;
						}
						else if (L"����" == HeadInfo.m_StrHeadNameCn)
						{
							HeadInfo.m_StrHeadNameCn = m_strSellPrice;
						}
						else if (L"����" == HeadInfo.m_StrHeadNameCn)
						{
							HeadInfo.m_StrHeadNameCn = m_strNewPrice;
						}
					}

					m_aReportHeadInfo[eReportType].Add(HeadInfo);
				}

				pHeadItemElement = pHeadItemElement->NextSiblingElement();
			}

			if ( m_aReportHeadInfo[eReportType].GetSize() <= 0 )
			{
				// �д���,����ΪĬ��ֵ:
				SetHeadDataToDefault(eReportType, m_aReportHeadInfo[eReportType], m_aFixCol[eReportType]);
			}
		}
		//
		pHeadInfoElment = pHeadInfoElment->NextSiblingElement();
	}
	//
	return true;
}

bool32 CReportScheme::SaveDataToXml()
{
	
	CString StrPath = GetPath();

	std::string StrPathA ;
	Unicode2MultiChar(CP_ACP, StrPath, StrPathA);
	//
	TCHAR TStrPath[2048];
	lstrcpy(TStrPath, StrPath);
	_tcheck_if_mkdir(TStrPath);
	//

	TiXmlDocument MyDoc = TiXmlDocument(StrPathA.c_str());

	CString StrXml		= L"";
	CString StrHeadInfo = L"";

	StrXml  = L"<?xml version =\"1.0\" encoding=\"utf-8\" standalone=\"yes\"?> \n";
	StrXml += L"<XMLDATA version=\"1.0\" app = \"ggtong\" data = \"GridHead\">\n";

	for(int32 i = 0; i < ERTCount; i++)
	{
		E_ReportType eReportType = (E_ReportType)i;
				
		StrHeadInfo.Format( L"<%s %s=\"%d\" %s=\"%d\" %s=\"%s\">\n",
							CString(KStrElementNameHeadInfo).GetBuffer(),
							CString(KStrElementAttriHeadType).GetBuffer(), i,
							CString(KStrElementAttriFixCol).GetBuffer(), m_aFixCol[i],
							CString(KStrElementAttriHeadName).GetBuffer(), GetReportTypeCnName(eReportType).GetBuffer() );
		
		for(int32 j = 0; j < m_aReportHeadInfo[eReportType].GetSize(); j++ )
		{
			CString StrHeadItem;
			
			StrHeadItem.Format( L"<%s %s=\"%s\" %s=\"%d\" %s=\"%d\"/>\n",
								CString(KStrElementNameHeadItem).GetBuffer(), 
								CString(KStrElementAttriHeadName).GetBuffer(),  m_aReportHeadInfo[eReportType].GetAt(j).m_StrHeadNameCn.GetBuffer(),
								CString(KStrElementAttriHeadWidth).GetBuffer(), m_aReportHeadInfo[eReportType].GetAt(j).m_iHeadWidth,
								CString(KStrElementAttriHeadColumnType).GetBuffer(),  m_aReportHeadInfo[eReportType].GetAt(j).m_eReportHeader
								);
			
			StrHeadInfo += StrHeadItem;
		}
		
		StrHeadInfo += L"</";
		StrHeadInfo += (CString)KStrElementNameHeadInfo;
		StrHeadInfo += L">\n";
		
		//
		StrXml += StrHeadInfo;
	}	
		
	StrXml += L"</XMLDATA>";
	//


	SaveXmlFile(StrPathA.c_str(), StrXml);

	return true;
}


void CReportScheme::SetPrivateFolderName(CString& strName)
{
	m_strPrivateName = strName;
}


bool32 T_HeadInfo::InitilizeHeadInfo( CReportScheme::E_ReportHeader eHeader )
{
	ASSERT( m_eReportHeader <= CReportScheme::ERHCount );
	m_eReportHeader = eHeader;
	m_StrHeadNameCn = CReportScheme::GetReportHeaderCnName(eHeader);
	m_iHeadWidth	= CReportScheme::GetReportColDefaultWidth(eHeader);
	return m_eReportHeader <= CReportScheme::ERHCount;
}
