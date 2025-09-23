#ifndef _ENUM_TYPE_H_
#define _ENUM_TYPE_H_
enum E_ReportType
{
	ERTFuturesCn = 0,		// �����ڻ�	
	ERTStockCn,				// ����֤ȯ
	ERTMony,				// �����Ŀ
	ERTExp,					// ָ����Ŀ
	ERTStockHk,				// �۹���Ŀ
	ERTFuturesForeign,		// �����ڻ�
		// 
	ERTWarrantCn,			// ����Ȩ֤
	ERTWarrantHk,			// ���Ȩ֤
	
	//
	ERTFutureRelaMonth,		// ������ڻ�
	ERTFutureSpot,			// �ֻ�
	ERTExpForeign,			// ����ָ��
	ERTFutureGold,			// �ƽ��ڻ�
	// 
	ERTCustom,				// �Զ���
	
	ERTNone,				// ������(�ձ��۱�) �����ڴ�֮���¼ӵı���С����Ŀ���öԻ�������ĸ�tab item���������ñ�ͷ
		// 
	ERTSelectStock,			// ����ѡ�ɱ�ͷ(������Ŀ��ʱ����)	// �����������λ��ȡ������ǰ��Noneλ�ã�������������˿ձ��۱�������
	
	ERTFinance,            // ���������ͷ
	
	ERTMerchSort,          // ����ǿ����ͷ

	ERTCapitalFlow, 
	ERTMainMasukura,

	//
	ERTCount				
};

// ����ʱ�䷽ʽ
typedef enum E_ReqTimeType
{
	ERTYSpecifyTime = 0,		// ָ��ʱ��
	ERTYFrontCount,				// ��������, ��ʱ�����ķ���
	ERTYCount					// ˳������, ��ʱ����µķ���
}E_ReqTimeType;

// ������
typedef enum E_RespErrorCode
{
	ERENone = 0,				// �޴���
	EREServer,					// ����������
	ERERequestError,			// �������
	EREInvalidArgu,				// ��������
	EREInvalidRight,			// �û��޴�����Ȩ��
	ERENoBigMarket = 20,		// δ�ҵ����г�
	ERENoMarket,				// δ�ҵ��г�
	ERENoSymbol,				// δ�ҵ���Ʒ
	ERENeedAuth = 30,			// Ϊ��֤
	EREAuthError,				// ��֤����

	ERENewVersion = 200,		// Э��汾����
	//
	ERECount
}E_RespErrorCode;

// ��������
typedef enum E_MerchReportField		
{
	EMRFRisePercent = 0,		// �ǵ���
	EMRFRiseValue,				// �ǵ�
	EMRFRiseRate,				// ����
	EMRFPriceNew,				// ���¼�
	EMRFPriceAverage,			// ����
	EMRFVolumeCur,				// ����
	EMRFPriceBuy,				// ���
	EMRFPriceSell,				// ����
	EMRFVolumeTotal,			// ����
	EMRFPriceOpen,				// ��
	EMRFPricePrevClose,			// ���ռ�
	EMRFPriceHigh,				// ��߼�
	EMRFPriceLow,				// ��ͼ�
	EMRFAmountTotal,			// �ܽ��
	EMRFAmplitude,				// ���
	EMRFVolRatio,				// ����
	EMRFBidRatio,				// ί��
	EMRFBidDifference,			// ί��
	EMRFIn,						// ����
	EMRFOut,					// ����
	EMRFInOutRatio,				// �����̱�	
	
	EMRFRisePercent5Min,		// 5�����Ƿ�
	EMRFDropPercent5Min,		// 5���ӵ���

	EMRFPricePreAvg,			// �����������
	EMRFHold,					// �ֲ�
	EMRFCurAmount,				// ���ʳɽ���
	EMRFCurHold,				// ����

	EMRFCapticalFlow,			// �ʽ�����
	EMRFPeRate,					// ��ӯ��
	EMRFTradeRate,				// ������
	EMRFIntensity,				// ǿ����

	// ������������
	EMRFAllStock,				// �ܹɱ�
	EMRFCircStock,				// ��ͨ�ɱ�
	EMRFAllAsset,				// ���ʲ�
	EMRFFlowDebt,				// ������ծ
	EMRFPerFund,				// ÿ�ɹ�����
	
	EMRFBusinessProfit,			// Ӫҵ����
	EMRFPerNoDistribute,		// ÿ��δ����
	EMRFPerIncomeYear,			// ÿ������(��)
	EMRFPerPureAsset,			// ÿ�ɾ��ʲ�
	EMRFChPerPureAsset,			// ����ÿ�ɾ��ʲ�
	
	EMRFDorRightRate,			// �ɶ�Ȩ���
	EMRFCircMarketValue,		// ��ͨ��ֵ
	EMRFAllMarketValue,			// ����ֵ
	

	//
	EMRFCount
}E_MerchReportField;

typedef enum E_OffKLineType
{
	EOKTVipData = 0,
	EOKTIndexData,
	EOKTOfflineData,

	EOKTCount
}E_OffKLineType;

// ���ݲ���,�����Ŀ���ԭ��
typedef enum E_ReConnectType
{
	ERCTWaitingForOnePack = 0,		// commengine ����һ������ʱ�䳬�� 30s , ��Ҫ����
	ERCTNoDataAtAll,				// commengine �г��� 30s û���յ�һ������, ��Ҫ����
	ERCTTooMuchTimeOutPack,			// dataengine ���г��� 3 ������ 30s û�лصİ�, ��Ҫ����
	ERCTPackTimeOut,				// dataengine ���г��� 5 ��û�лصİ�, �ͻ�����ʾ���粻��

	ERCTCount,
}E_ReConnectType;
#endif
