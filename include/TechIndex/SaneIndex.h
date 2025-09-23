#ifndef _SANE_INDEX_H
#define _SANE_INDEX_H

//#include "IoViewShare.h"
#include <float.h>
#include "ShareFun.h"
#include "SelectStockStruct.h"
#include "TechExport.h"

#define SANE_INDEX_MOVE_EMA5		(int32)5					// EMA5 的平移
#define SANE_INDEX_MOVE_EMA8		(int32)8					// EMA8 的平移
#define SANE_INDEX_MOVE_EMA13		(int32)8					// EMA13的平移
#define SANE_INDEX_MOVE_MAX			(int32)8					// 平移的最大值

#define SANE_INDEX_WEIGHT_EMA5		(float)0.2					// EMA5 的权重
#define SANE_INDEX_WEIGHT_EMA8		(float)0.3					// EMA8 的权重
#define SANE_INDEX_WEIGHT_EMA13		(float)0.5					// EMA13的权重

#define SANE_INDEX_KLINE_NUMS		(int32)13					// 算K 线高度平均值时需要的节点个数
#define SANE_INDEX_KLINE_AVG_DEF	(float)0.05					// 如果平均高度是 0 的时候,默认平均高度为 0.05


typedef struct T_SaneNodeDebugInfo								// 调试信息
{
	int32		m_iNodePos;										// 节点的序号
	int32		m_iCondition;									// 满足,违反的条件序号
	bool32		m_bShake;										// 是否震荡

	T_SaneNodeDebugInfo()
	{
		m_iNodePos		= -1;
		m_iCondition	= -1;
		m_bShake		= false;
	}

}T_SaneNodeDebugInfo;

// 绘图时的参数结构
typedef struct T_DrawSaneParam
{
	CPoint  m_CenterPoint;		// K 线的中心点
	int32   m_iLow;				// K 线的下边界
	int32	m_iHigh;			// K 线的上边界

}T_DrawSaneParam;

// 鼠标停靠到标志上,显示Tips
typedef struct T_SaneNodeTip
{
	CRect		m_RectSign;
	int32		m_iSaneIndex; // 指向的K线视图里面的Sane index
	CString		m_StrTip;

}T_SaneNodeTip;

//
enum E_SaneIntensity			// 节点强度
{
	ESISTRONG = 0,				// 强
	ESINORMAL,					// 一般
	ESIWEEK,					// 弱

	ESICOUNT,
};

// 状态 
enum E_SaneIndexState
{
	ESISNONE = 0,
	ESISKCGW,					// 空仓观望
	ESISDTOC,					// 多头开仓
	ESISDTCY,					// 多头持有
	ESISDTCC,					// 多头清仓	
	ESISDCKO,					// 多头清仓,空头开仓
	ESISKTOC,					// 空头开仓
	ESISKTCY,					// 空头持有
	ESISKTCC,					// 空头清仓
	ESISKCDO,					// 空头清仓,多头开仓

	ESISCOUNT
};

// 多头市场,空头市场
enum E_SaneMarketInfo
{	
	ESMIDT =  0,				// 多头市场
	ESMIDH,						// 多头回调
	ESMIKT,						// 空头市场
	ESMIKH,						// 空头回调
	
	ESMICOUNT
};
 
typedef struct T_SaneIndexNode 
{
	E_SaneIndexState		m_eSaneIndexState;		// 节点状态
	E_SaneMarketInfo		m_eSaneMarketInfo;		// 市场信息
	float					m_fIntensity;			// 强度值					// 从指标中取数据
	float					m_fStability;			// 稳定值
	bool32					m_bStability;			// 是否稳定
	float					m_fLineData;			// 趋势MA					// 从指标中取数据(用于画线)	
	CTime					m_TimeKLine;			// 对应的K 线节点的时间
	CTime					m_TimeAction;			// 提示交易的时间
	float					m_fPrice;				// 对应的价格
	float					m_fTrendValue;			// 趋势的值 MAQSMAC20
	float					m_fZhisun;				// 买入时对应的止损价格
	T_SaneNodeDebugInfo		m_stDebugInfo;			// 调试信息

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

// 得到节点的Tip
EXPORT_API CString GetNodeTip(bool32 bFuture, int32 iSaveDec, const T_SaneIndexNode& Node);

// 从指标里面取数据:// 取指标序列
EXPORT_API bool32	GetIndex(const CString& StrLineName, IN const T_IndexOutArray* pIndexIn, OUT float*& pIndex, OUT int32& iNumPoint);

// 取指标值	
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
EXPORT_API float	CalcSaneIndexStabilityValue(float fIntensityNow, float fIntensityPre);						// 算稳定性
EXPORT_API E_SaneIntensity  BeStrong(const T_SaneIndexNode& Node);														// 判断强度	
EXPORT_API bool32	BeStabilityValue(int32 iPos, IN const T_IndexOutArray* pIndex);							// 是否稳定

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

// 取高, 低点
EXPORT_API float	GetHighLowPoint(const float* pData, int32 iCountData, int32 iNum1, bool32 bHigh);

// 算稳定性
EXPORT_API float	CalcSaneIndexStabilityValue(float fIntensityNow, float fIntensityPre);

// 判断强度
E_SaneIntensity  EXPORT_API BeStrong(const T_SaneIndexNode& Node);	

// 是否稳定
bool32			 BeStabilityValue(int32 iPos, IN const T_IndexOutArray* pIndex);

// 判断开仓条件和清仓条件是否对应
EXPORT_API bool32	BeRightOpenCondition(bool32 bFuture, bool32 bDtoc, int32 iOpenCondition, int32 iClearCondition);			

// 得到市场信息
EXPORT_API E_SaneMarketInfo GetMarketInfo(int32 iPos, IN T_IndexOutArray* pIndex);										// 得到市场信息

// 得到节点状态字符串
EXPORT_API CString GetNodeStateString(bool32 bFuture, const T_SaneIndexNode& Node);

// 得到市场信息字符串
EXPORT_API CString GetNodeMarketString(E_SaneMarketInfo eMarketInfo);

// 强度字符串
EXPORT_API CString GetSaneNodeIntensityString(const T_SaneIndexNode& Node);

// 稳定性字符串	
EXPORT_API CString GetSaneNodeStabilityString(const T_SaneIndexNode& Node);

// 趋势字符串	
EXPORT_API CString GetSaneNodeUpDownString(const T_SaneIndexNode& Node);

// 得到这个节点的描述字符串
EXPORT_CLASS CString GetSaneNodeSummary(const T_SaneIndexNode& Node);
// 统计函数
/*
bool32			StatisticaSaneValues( IN const CArray<T_SaneIndexNode, T_SaneIndexNode>& aSaneValues,		// 传入指标节点序列
									  OUT int32& iTradeTimes,												// 总交易次数		
									  OUT int32& iGoodTrades,												// 盈利交易
								      OUT int32& iBadTrades,											    // 亏损交易	
								      OUT float& fAccuracyRate,												// 准确率
								      OUT float& fProfitability);											// 收益率
*/
EXPORT_CLASS bool32 StatisticaSaneValues2( IN  int32 iKLineNums,												// 算多少根K 线
									       IN  float fPriceLastClose,											// 最后一根K 线的收盘价(防止最新一根K 线没到收盘时间, 没有更新, 复制前一根指标节点.导致计算的数据不是实时)		
									       OUT int32& iKLineNumsReal,											// 实际算了多少根
									       IN  const CArray<T_SaneIndexNode, T_SaneIndexNode>& aSaneValues,		// 传入指标节点序列
									       OUT int32& iTradeTimes,												// 总交易次数		
									       OUT int32& iGoodTrades,												// 盈利交易
									       OUT int32& iBadTrades,											    // 亏损交易	
									       OUT float& fAccuracyRate,											// 准确率
									       OUT float& fProfitability,											// 收益率
									       OUT float& fProfit);													// 具体收益	

// 计算EMA 指标的值
EXPORT_CLASS T_IndexOutArray*	CalcIndexEMAForSane(const CArray<CKLine, CKLine>& aKLines);																																			

// 更新最新一个的趋势指标值
EXPORT_CLASS bool32	UpDateLatestIndexForSane(const T_IndexOutArray* pIndex, OUT CArray<T_SaneIndexNode,T_SaneIndexNode>& aSaneIndexValues);

// 计算历史数据
EXPORT_CLASS bool32	CalcHistorySaneIndex(IN bool32 bFuture, IN bool32 bPassedUpdateTime, IN T_IndexOutArray* pIndex, IN const CArray<CKLine, CKLine>& aKLines, OUT CArray<T_SaneIndexNode,T_SaneIndexNode>& aSaneIndexValues);											

// 计算实时数据
EXPORT_CLASS bool32	CalcLatestSaneIndex(IN  bool32 bAddNew, IN bool32 bFuture, IN T_IndexOutArray* pIndex, IN const CArray<CKLine, CKLine>& aKLines, OUT CArray<T_SaneIndexNode,T_SaneIndexNode>& aSaneIndexValues, const CTime& TimeAction, OUT bool32& bKillTimer);	

// 根据前一根K 线计算当前K 线
EXPORT_CLASS bool32	CalcSaneNodeAccordingPreNode(bool32 bHis, IN bool32 bFuture, int32 iPos, IN T_IndexOutArray* pIndex, IN const CArray<CKLine, CKLine>& aKLines, CArray<T_SaneIndexNode,T_SaneIndexNode>& aSaneIndexValues, const CTime& TimeAction = 0);			

// 判断条件
EXPORT_CLASS bool32	CalcConditionSaneDTOC(bool32 bFuture, int32 iPos, bool32 bHis, T_IndexOutArray* pIndex, const CArray<CKLine, CKLine>& aKLines, CArray<T_SaneIndexNode, T_SaneIndexNode>& aIndexNodes);
EXPORT_CLASS bool32	CalcConditionSaneDTCC(bool32 bFuture, int32 iPos, int32 iDtocConditions, T_IndexOutArray* pIndex, const CArray<CKLine, CKLine>& aKLines, CArray<T_SaneIndexNode,T_SaneIndexNode>& aSaneIndexValues);

EXPORT_CLASS bool32	CalcConditionSaneKTOC(bool32 bFuture, int32 iPos, bool32 bHis, T_IndexOutArray* pIndex, const CArray<CKLine, CKLine>& aKLines, CArray<T_SaneIndexNode, T_SaneIndexNode>& aIndexNodes);
EXPORT_CLASS bool32	CalcConditionSaneKTCC(bool32 bFuture, int32 iPos, int32 iKtocConditions, T_IndexOutArray* pIndex, const CArray<CKLine, CKLine>& aKLines, CArray<T_SaneIndexNode,T_SaneIndexNode>& aSaneIndexValues);

// 是否振荡开仓
EXPORT_CLASS bool32	JudgeShakeOC(bool32 bFuture, int32 iPos, IN const T_IndexOutArray* pIndex);

#endif // _SANE_INDEX_H