#ifndef _SANE_INDEX_H
#define _SANE_INDEX_H

//#include "IoViewShare.h"
#include <float.h>
#include "ShareFun.h"
#include "SelectStockStruct.h"
#include "TechExport.h"

#define SANE_INDEX_MOVE_EMA5		(int32)5					// EMA5 ��ƽ��
#define SANE_INDEX_MOVE_EMA8		(int32)8					// EMA8 ��ƽ��
#define SANE_INDEX_MOVE_EMA13		(int32)8					// EMA13��ƽ��
#define SANE_INDEX_MOVE_MAX			(int32)8					// ƽ�Ƶ����ֵ

#define SANE_INDEX_WEIGHT_EMA5		(float)0.2					// EMA5 ��Ȩ��
#define SANE_INDEX_WEIGHT_EMA8		(float)0.3					// EMA8 ��Ȩ��
#define SANE_INDEX_WEIGHT_EMA13		(float)0.5					// EMA13��Ȩ��

#define SANE_INDEX_KLINE_NUMS		(int32)13					// ��K �߸߶�ƽ��ֵʱ��Ҫ�Ľڵ����
#define SANE_INDEX_KLINE_AVG_DEF	(float)0.05					// ���ƽ���߶��� 0 ��ʱ��,Ĭ��ƽ���߶�Ϊ 0.05


typedef struct T_SaneNodeDebugInfo								// ������Ϣ
{
	int32		m_iNodePos;										// �ڵ�����
	int32		m_iCondition;									// ����,Υ�����������
	bool32		m_bShake;										// �Ƿ���

	T_SaneNodeDebugInfo()
	{
		m_iNodePos		= -1;
		m_iCondition	= -1;
		m_bShake		= false;
	}

}T_SaneNodeDebugInfo;

// ��ͼʱ�Ĳ����ṹ
typedef struct T_DrawSaneParam
{
	CPoint  m_CenterPoint;		// K �ߵ����ĵ�
	int32   m_iLow;				// K �ߵ��±߽�
	int32	m_iHigh;			// K �ߵ��ϱ߽�

}T_DrawSaneParam;

// ���ͣ������־��,��ʾTips
typedef struct T_SaneNodeTip
{
	CRect		m_RectSign;
	int32		m_iSaneIndex; // ָ���K����ͼ�����Sane index
	CString		m_StrTip;

}T_SaneNodeTip;

//
enum E_SaneIntensity			// �ڵ�ǿ��
{
	ESISTRONG = 0,				// ǿ
	ESINORMAL,					// һ��
	ESIWEEK,					// ��

	ESICOUNT,
};

// ״̬ 
enum E_SaneIndexState
{
	ESISNONE = 0,
	ESISKCGW,					// �ղֹ���
	ESISDTOC,					// ��ͷ����
	ESISDTCY,					// ��ͷ����
	ESISDTCC,					// ��ͷ���	
	ESISDCKO,					// ��ͷ���,��ͷ����
	ESISKTOC,					// ��ͷ����
	ESISKTCY,					// ��ͷ����
	ESISKTCC,					// ��ͷ���
	ESISKCDO,					// ��ͷ���,��ͷ����

	ESISCOUNT
};

// ��ͷ�г�,��ͷ�г�
enum E_SaneMarketInfo
{	
	ESMIDT =  0,				// ��ͷ�г�
	ESMIDH,						// ��ͷ�ص�
	ESMIKT,						// ��ͷ�г�
	ESMIKH,						// ��ͷ�ص�
	
	ESMICOUNT
};
 
typedef struct T_SaneIndexNode 
{
	E_SaneIndexState		m_eSaneIndexState;		// �ڵ�״̬
	E_SaneMarketInfo		m_eSaneMarketInfo;		// �г���Ϣ
	float					m_fIntensity;			// ǿ��ֵ					// ��ָ����ȡ����
	float					m_fStability;			// �ȶ�ֵ
	bool32					m_bStability;			// �Ƿ��ȶ�
	float					m_fLineData;			// ����MA					// ��ָ����ȡ����(���ڻ���)	
	CTime					m_TimeKLine;			// ��Ӧ��K �߽ڵ��ʱ��
	CTime					m_TimeAction;			// ��ʾ���׵�ʱ��
	float					m_fPrice;				// ��Ӧ�ļ۸�
	float					m_fTrendValue;			// ���Ƶ�ֵ MAQSMAC20
	float					m_fZhisun;				// ����ʱ��Ӧ��ֹ��۸�
	T_SaneNodeDebugInfo		m_stDebugInfo;			// ������Ϣ

	T_SaneIndexNode()
	{
		m_eSaneIndexState	= ESISNONE;
		m_eSaneMarketInfo	= ESMICOUNT;

		m_fIntensity		= FLT_MIN;
		m_fStability		= FLT_MIN;
		m_fLineData		= FLT_MIN;

		m_TimeKLine			= -1;
		m_TimeAction		= -1;

		m_fTrendValue		= FLT_MIN;
		m_fPrice			= FLT_MIN;		 
		m_fZhisun			= FLT_MIN;		 
	};
	
}T_SaneIndexNode;

// �õ��ڵ��Tip
EXPORT_API CString GetNodeTip(bool32 bFuture, int32 iSaveDec, const T_SaneIndexNode& Node);

// ��ָ������ȡ����:// ȡָ������
EXPORT_API bool32	GetIndex(const CString& StrLineName, IN const T_IndexOutArray* pIndexIn, OUT float*& pIndex, OUT int32& iNumPoint);

// ȡָ��ֵ	
EXPORT_API float	GetIndexValue(int32 iPos, const CString& StrLineName, IN const T_IndexOutArray* pIndex);
		
EXPORT_API float	GetchlFromIndex(int32 iPos, IN const T_IndexOutArray* pIndex);								
EXPORT_API float	Getm1FromIndex(int32 iPos, IN const T_IndexOutArray* pIndex);	
EXPORT_API float	Getm2FromIndex(int32 iPos, IN const T_IndexOutArray* pIndex);	
EXPORT_API float	Getm3FromIndex(int32 iPos, IN const T_IndexOutArray* pIndex);	
EXPORT_API float	Getm4FromIndex(int32 iPos, IN const T_IndexOutArray* pIndex);	
EXPORT_API float	Getm5FromIndex(int32 iPos, IN const T_IndexOutArray* pIndex);	
EXPORT_API float	Getm6FromIndex(int32 iPos, IN const T_IndexOutArray* pIndex);	
EXPORT_API float	Getm7FromIndex(int32 iPos, IN const T_IndexOutArray* pIndex);	
EXPORT_API float	Getm8FromIndex(int32 iPos, IN const T_IndexOutArray* pIndex);	

EXPORT_API float	Getma5FromIndex(int32 iPos, IN const T_IndexOutArray* pIndex);								
EXPORT_API float	Getma8FromIndex(int32 iPos, IN const T_IndexOutArray* pIndex);								
EXPORT_API float	Getma13FromIndex(int32 iPos, IN const T_IndexOutArray* pIndex);								
EXPORT_API float	Getma21FromIndex(int32 iPos, IN const T_IndexOutArray* pIndex);								
EXPORT_API float	Getma34FromIndex(int32 iPos, IN const T_IndexOutArray* pIndex);	
EXPORT_API float	Getma55FromIndex(int32 iPos, IN const T_IndexOutArray* pIndex);								
EXPORT_API float	Getma89FromIndex(int32 iPos, IN const T_IndexOutArray* pIndex);	
EXPORT_API float	Getlma5FromIndex(int32 iPos, IN const T_IndexOutArray* pIndex);								
EXPORT_API float	Gethma5FromIndex(int32 iPos, IN const T_IndexOutArray* pIndex);								
EXPORT_API float	GethighkFromIndex(int32 iPos, IN const T_IndexOutArray* pIndex);								
EXPORT_API float	GethhcFromIndex(int32 iPos, IN const T_IndexOutArray* pIndex);								
EXPORT_API float	GetllcFromIndex(int32 iPos, IN const T_IndexOutArray* pIndex);								
EXPORT_API float	GethighkkFromIndex(int32 iPos, IN const T_IndexOutArray* pIndex);								
						

EXPORT_API float	Getkhigh1FromIndex(int32 iPos, IN const T_IndexOutArray* pIndex);		
EXPORT_API float	GetkhighFromIndex(int32 iPos, IN const T_IndexOutArray* pIndex);		
EXPORT_API float	Getmahighk5FromIndex(int32 iPos, IN const T_IndexOutArray* pIndex);								
EXPORT_API float	Getkhigh01FromIndex(int32 iPos, IN const T_IndexOutArray* pIndex);								
EXPORT_API float	Getkhigh02FromIndex(int32 iPos, IN const T_IndexOutArray* pIndex);								
EXPORT_API float	GetqsmaFromIndex(int32 iPos, IN const T_IndexOutArray* pIndex);								
EXPORT_API float	Getdif89FromIndex(int32 iPos, IN const T_IndexOutArray* pIndex);								
EXPORT_API float	Getdif21FromIndex(int32 iPos, IN const T_IndexOutArray* pIndex);								
EXPORT_API float	Getdif89cFromIndex(int32 iPos, IN const T_IndexOutArray* pIndex);								
EXPORT_API float	Getdif21cFromIndex(int32 iPos, IN const T_IndexOutArray* pIndex);								
EXPORT_API float	GetqsqdFromIndex(int32 iPos, IN const T_IndexOutArray* pIndex);								
EXPORT_API float	GetmaqsqdFromIndex(int32 iPos, IN const T_IndexOutArray* pIndex);								
EXPORT_API float	GetqsqdcFromIndex(int32 iPos, IN const T_IndexOutArray* pIndex);								
EXPORT_API float	GetmaqsqdcFromIndex(int32 iPos, IN const T_IndexOutArray* pIndex);								
EXPORT_API float	GetzhiyingFromIndex(int32 iPos, IN const T_IndexOutArray* pIndex);								
EXPORT_API float	GetzhisunFromIndex(int32 iPos, IN const T_IndexOutArray* pIndex);								
EXPORT_API float	GetqsqdzdFromIndex(int32 iPos, IN const T_IndexOutArray* pIndex);
EXPORT_API float	GethlmaqsqdcFromIndex(int32 iPos, IN const T_IndexOutArray* pIndex);
EXPORT_API float	Gethhv13FromIndex(int32 iPos, IN const T_IndexOutArray* pIndex);
EXPORT_API float	Getllv13FromIndex(int32 iPos, IN const T_IndexOutArray* pIndex);
EXPORT_API float	Getllv5FromIndex(int32 iPos, IN const T_IndexOutArray* pIndex);
EXPORT_API float	Getdif89c6FromIndex(int32 iPos, IN const T_IndexOutArray* pIndex);
EXPORT_API float	GetqsqdbFromIndex(int32 iPos, IN const T_IndexOutArray* pIndex);
EXPORT_API float	Gethdif21FromIndex(int32 iPos, IN const T_IndexOutArray* pIndex);
EXPORT_API float	GetdifcdcFromIndex(int32 iPos, IN const T_IndexOutArray* pIndex);
EXPORT_API float	GetmaxdifFromIndex(int32 iPos, IN const T_IndexOutArray* pIndex);
EXPORT_API float	Getmindif89FromIndex(int32 iPos, IN const T_IndexOutArray* pIndex);
EXPORT_API float	Getlow5FromIndex(int32 iPos, IN const T_IndexOutArray* pIndex);
EXPORT_API float	Getlow8FromIndex(int32 iPos, IN const T_IndexOutArray* pIndex);
EXPORT_API float	Gethigh5FromIndex(int32 iPos, IN const T_IndexOutArray* pIndex);
EXPORT_API float	Gethigh8FromIndex(int32 iPos, IN const T_IndexOutArray* pIndex);

EXPORT_API float	GetCfjd0FromIndex(int32 iPos, IN const T_IndexOutArray* pIndex);								
EXPORT_API float	GetCfjd1FromIndex(int32 iPos, IN const T_IndexOutArray* pIndex);								
EXPORT_API float	GetCfjd2FromIndex(int32 iPos, IN const T_IndexOutArray* pIndex);								
EXPORT_API float	GetMaxCfjdFromIndex(int32 iPos, IN const T_IndexOutArray* pIndex);
EXPORT_API float	GetMinCfjdFromIndex(int32 iPos, IN const T_IndexOutArray* pIndex);	
//
EXPORT_API float	CalcSaneIndexStabilityValue(float fIntensityNow, float fIntensityPre);						// ���ȶ���
EXPORT_API E_SaneIntensity  BeStrong(const T_SaneIndexNode& Node);														// �ж�ǿ��	
EXPORT_API bool32	BeStabilityValue(int32 iPos, IN const T_IndexOutArray* pIndex);							// �Ƿ��ȶ�

EXPORT_API float	GetMaxMaFromIndex(int32 iPos, IN const T_IndexOutArray* pIndex);
EXPORT_API float	GetMinMaFromIndex(int32 iPos, IN const T_IndexOutArray* pIndex);
EXPORT_API float	GetMaxMacFromIndex(int32 iPos, IN const T_IndexOutArray* pIndex);

EXPORT_API float	GetZdtj1FromIndex(int32 iPos, IN const T_IndexOutArray* pIndex);
EXPORT_API float	GetZdtj2FromIndex(int32 iPos, IN const T_IndexOutArray* pIndex);

EXPORT_API float	Gethigh16FromIndex(int32 iPos, IN const T_IndexOutArray* pIndex);
EXPORT_API float	Gethigh4FromIndex(int32 iPos, IN const T_IndexOutArray* pIndex);
EXPORT_API float	Getlow16FromIndex(int32 iPos, IN const T_IndexOutArray* pIndex);
EXPORT_API float	Getlow4FromIndex(int32 iPos, IN const T_IndexOutArray* pIndex);
EXPORT_API float	Getlow3FromIndex(int32 iPos, IN const T_IndexOutArray* pIndex);
EXPORT_API float	Getzsj0FromIndex(int32 iPos, IN const T_IndexOutArray* pIndex);
EXPORT_API float	GetzsjFromIndex(int32 iPos, IN const T_IndexOutArray* pIndex);

EXPORT_API float	GetqsqdcFromIndex(int32 iPos, IN const T_IndexOutArray* pIndex);			
EXPORT_API float	GetqsmaFromIndex(int32 iPos, IN const T_IndexOutArray* pIndex);


EXPORT_API float	GetcfjdcFromIndex(int32 iPos, IN const T_IndexOutArray* pIndex);
EXPORT_API float	Getma34cFromIndex(int32 iPos, IN const T_IndexOutArray* pIndex);
EXPORT_API float	Getma89cFromIndex(int32 iPos, IN const T_IndexOutArray* pIndex);
EXPORT_API float	Getqsma5FromIndex(int32 iPos, IN const T_IndexOutArray* pIndex);
EXPORT_API float	GetbiasFromIndex(int32 iPos, IN const T_IndexOutArray* pIndex);
EXPORT_API float	Getbias1FromIndex(int32 iPos, IN const T_IndexOutArray* pIndex);
EXPORT_API float	GetmabiasFromIndex(int32 iPos, IN const T_IndexOutArray* pIndex);
EXPORT_API float	Getmabias21FromIndex(int32 iPos, IN const T_IndexOutArray* pIndex);
EXPORT_API float	GetbiascFromIndex(int32 iPos, IN const T_IndexOutArray* pIndex);
EXPORT_API float	GetmabiascFromIndex(int32 iPos, IN const T_IndexOutArray* pIndex);
EXPORT_API float	Getllvmabias3FromIndex(int32 iPos, IN const T_IndexOutArray* pIndex);
EXPORT_API float	GetcfjdllvcFromIndex(int32 iPos, IN const T_IndexOutArray* pIndex);
EXPORT_API float	GetcfjdhhvcFromIndex(int32 iPos, IN const T_IndexOutArray* pIndex);
EXPORT_API float	Getcfjd21cFromIndex(int32 iPos, IN const T_IndexOutArray* pIndex);

EXPORT_API float	Getllvmabias8FromIndex(int32 iPos, IN const T_IndexOutArray* pIndex);
EXPORT_API float	Gethhvmabias3FromIndex(int32 iPos, IN const T_IndexOutArray* pIndex);

// ȡ��, �͵�
EXPORT_API float	GetHighLowPoint(const float* pData, int32 iCountData, int32 iNum1, bool32 bHigh);

// ���ȶ���
EXPORT_API float	CalcSaneIndexStabilityValue(float fIntensityNow, float fIntensityPre);

// �ж�ǿ��
E_SaneIntensity  EXPORT_API BeStrong(const T_SaneIndexNode& Node);	

// �Ƿ��ȶ�
bool32			 BeStabilityValue(int32 iPos, IN const T_IndexOutArray* pIndex);

// �жϿ�����������������Ƿ��Ӧ
EXPORT_API bool32	BeRightOpenCondition(bool32 bFuture, bool32 bDtoc, int32 iOpenCondition, int32 iClearCondition);			

// �õ��г���Ϣ
EXPORT_API E_SaneMarketInfo GetMarketInfo(int32 iPos, IN T_IndexOutArray* pIndex);										// �õ��г���Ϣ

// �õ��ڵ�״̬�ַ���
EXPORT_API CString GetNodeStateString(bool32 bFuture, const T_SaneIndexNode& Node);

// �õ��г���Ϣ�ַ���
EXPORT_API CString GetNodeMarketString(E_SaneMarketInfo eMarketInfo);

// ǿ���ַ���
EXPORT_API CString GetSaneNodeIntensityString(const T_SaneIndexNode& Node);

// �ȶ����ַ���	
EXPORT_API CString GetSaneNodeStabilityString(const T_SaneIndexNode& Node);

// �����ַ���	
EXPORT_API CString GetSaneNodeUpDownString(const T_SaneIndexNode& Node);

// �õ�����ڵ�������ַ���
EXPORT_CLASS CString GetSaneNodeSummary(const T_SaneIndexNode& Node);
// ͳ�ƺ���
/*
bool32			StatisticaSaneValues( IN const CArray<T_SaneIndexNode, T_SaneIndexNode>& aSaneValues,		// ����ָ��ڵ�����
									  OUT int32& iTradeTimes,												// �ܽ��״���		
									  OUT int32& iGoodTrades,												// ӯ������
								      OUT int32& iBadTrades,											    // ������	
								      OUT float& fAccuracyRate,												// ׼ȷ��
								      OUT float& fProfitability);											// ������
*/
EXPORT_CLASS bool32 StatisticaSaneValues2( IN  int32 iKLineNums,												// ����ٸ�K ��
									       IN  float fPriceLastClose,											// ���һ��K �ߵ����̼�(��ֹ����һ��K ��û������ʱ��, û�и���, ����ǰһ��ָ��ڵ�.���¼�������ݲ���ʵʱ)		
									       OUT int32& iKLineNumsReal,											// ʵ�����˶��ٸ�
									       IN  const CArray<T_SaneIndexNode, T_SaneIndexNode>& aSaneValues,		// ����ָ��ڵ�����
									       OUT int32& iTradeTimes,												// �ܽ��״���		
									       OUT int32& iGoodTrades,												// ӯ������
									       OUT int32& iBadTrades,											    // ������	
									       OUT float& fAccuracyRate,											// ׼ȷ��
									       OUT float& fProfitability,											// ������
									       OUT float& fProfit);													// ��������	

// ����EMA ָ���ֵ
EXPORT_CLASS T_IndexOutArray*	CalcIndexEMAForSane(const CArray<CKLine, CKLine>& aKLines);																																			

// ��������һ��������ָ��ֵ
EXPORT_CLASS bool32	UpDateLatestIndexForSane(const T_IndexOutArray* pIndex, OUT CArray<T_SaneIndexNode,T_SaneIndexNode>& aSaneIndexValues);

// ������ʷ����
EXPORT_CLASS bool32	CalcHistorySaneIndex(IN bool32 bFuture, IN bool32 bPassedUpdateTime, IN T_IndexOutArray* pIndex, IN const CArray<CKLine, CKLine>& aKLines, OUT CArray<T_SaneIndexNode,T_SaneIndexNode>& aSaneIndexValues);											

// ����ʵʱ����
EXPORT_CLASS bool32	CalcLatestSaneIndex(IN  bool32 bAddNew, IN bool32 bFuture, IN T_IndexOutArray* pIndex, IN const CArray<CKLine, CKLine>& aKLines, OUT CArray<T_SaneIndexNode,T_SaneIndexNode>& aSaneIndexValues, const CTime& TimeAction, OUT bool32& bKillTimer);	

// ����ǰһ��K �߼��㵱ǰK ��
EXPORT_CLASS bool32	CalcSaneNodeAccordingPreNode(bool32 bHis, IN bool32 bFuture, int32 iPos, IN T_IndexOutArray* pIndex, IN const CArray<CKLine, CKLine>& aKLines, CArray<T_SaneIndexNode,T_SaneIndexNode>& aSaneIndexValues, const CTime& TimeAction = 0);			

// �ж�����
EXPORT_CLASS bool32	CalcConditionSaneDTOC(bool32 bFuture, int32 iPos, bool32 bHis, T_IndexOutArray* pIndex, const CArray<CKLine, CKLine>& aKLines, CArray<T_SaneIndexNode, T_SaneIndexNode>& aIndexNodes);
EXPORT_CLASS bool32	CalcConditionSaneDTCC(bool32 bFuture, int32 iPos, int32 iDtocConditions, T_IndexOutArray* pIndex, const CArray<CKLine, CKLine>& aKLines, CArray<T_SaneIndexNode,T_SaneIndexNode>& aSaneIndexValues);

EXPORT_CLASS bool32	CalcConditionSaneKTOC(bool32 bFuture, int32 iPos, bool32 bHis, T_IndexOutArray* pIndex, const CArray<CKLine, CKLine>& aKLines, CArray<T_SaneIndexNode, T_SaneIndexNode>& aIndexNodes);
EXPORT_CLASS bool32	CalcConditionSaneKTCC(bool32 bFuture, int32 iPos, int32 iKtocConditions, T_IndexOutArray* pIndex, const CArray<CKLine, CKLine>& aKLines, CArray<T_SaneIndexNode,T_SaneIndexNode>& aSaneIndexValues);

// �Ƿ��񵴿���
EXPORT_CLASS bool32	JudgeShakeOC(bool32 bFuture, int32 iPos, IN const T_IndexOutArray* pIndex);

#endif // _SANE_INDEX_H