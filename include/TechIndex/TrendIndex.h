#ifndef _TRENDINDEX_H
#define _TRENDINDEX_H

#include <float.h>
//#include "IoViewShare.h"
#include "ShareFun.h"
#include "CFormularContent.h"
#include "SelectStockStruct.h"
#include "TechExport.h"


//#define	STRONGTREND		0.6		// 强的趋势值
//#define	WEEKTREND		0.3		// 弱的趋势值
//#define	STABILITYTREND	15		// 稳定性
#define VALIDTIMES			3		// 能够容忍的不符合当前操作提示的次数
#define TRENDINTENSITYNUMS  13	    // 算趋势强度的时候,取的节点个数
#define SECONDSTOUPDATE		10		// K 线结束前多少秒更新指标值
//#define MA5MOVENUMS		3		// MA5 的移动间隔数
//#define MA8MOVENUMS		5		// MA8 的移动间隔数
//#define MA13MOVENUMS		8		// MA13 的移动间隔数

//#define MAXMOVENUMS	    8		// 最大的间隔数(上面几个值的最大值)

const CString gkStrTrendIndexFileName = _T("c:/TrendIndex.ini");

typedef struct T_TrendIndexParam
{
	float	m_fStrongTrend;		// 强的趋势值
	float	m_fWeekTrend;		// 弱的趋势值
	float	m_fOCTrend;			// 买卖开仓提示里的趋势强度值

	float	m_fStabilityTrend;	// 稳定性
	
	int32	m_iMa5MoveNums;		// MA5 的移动间隔数
	int32	m_iMa8MoveNums;		// MA8 的移动间隔数
	int32	m_iMa13MoveNums;	// MA13 的移动间隔数

	float	m_fMA5Weight;		// MA5 权重
	float	m_fMA8Weight;		// MA8 权重
	float	m_fMA13Weight;		// MA13 权重

	int32	m_iMaxMoveNums;		// 最大的间隔数(上面几个值的最大值)

	int32	m_iCdt5KLineNums;	// 条件5 中K 线个数
	float	m_fCdt5Times;		// 条件5 中倍数

}T_TrendIndexParam;

// 绘图时的参数结构
typedef struct T_DrawTrendParam
{
	CPoint  m_CenterPoint;		// K 线的中心点
	int32   m_iLow;				// K 线的下边界
	int32	m_iHigh;			// K 线的上边界

}T_DrawTrendParam;

extern T_TrendIndexParam g_TrendIndexParam;
//
enum E_TrendIntensity			// 节点强度
{
	ETISTRONG = 0,				// 强
	ETINORMAL,					// 一般
	ETIWEEK,					// 弱
};
//
enum E_HoldState				// 持仓状态
{
	EHSNONE = 0,				// 无状态,错误
	EHSKCGW,					// 空仓观望
	EHSDTCY,					// 多头持有
	EHSDTJC,					// 多头减仓
	EHSKTCY,					// 空头持有
	EHSKTJC,					// 空头减仓
	EHSCOUNT,
};

enum E_ActionPrompt				// 操作提示
{
	EAPNONE = 0,				// 无提示
	EAPDTOC,					// 多头开仓		
	EAPDTAC,					// 多头加仓
	EAPDTRC,					// 多头减仓	
	EAPDTCC,					// 多头清仓	
	EAPKTOC,					// 空头开仓
	EAPKTAC,					// 空头加仓
	EAPKTRC,					// 空头减仓
	EAPKTCC,					// 空头清仓	
	EAPCOUNT,
};

typedef struct T_TrendIndexNode 
{
	E_HoldState		m_eHoldState;			// 持仓 状态
	E_ActionPrompt  m_eActionPrompt;		// 操作 提示
	float			m_fTrendIntensity;		// 趋势 强度
	float			m_fTrendStability;		// 趋势 稳定性
	float			m_fMAWeighted;			// 趋势 MA 值(加权MA)
	float			m_fMAWeightedDif;		// 趋势 MA 加权差
	
	CTime			m_TimeKLine;			// 对应的K 线节点的时间
	CTime			m_TimeAction;			// 提示交易的时间
	float			m_fPrice;				// 对应的价格
	float			m_fMA5;					// MA5 的值(偏移后的)
	float			m_fMA8;					// MA8 的值
	float			m_fMA13;				// MA13 的值
	
	T_TrendIndexNode()
	{
		m_eHoldState		= EHSNONE;
		m_eActionPrompt		= EAPNONE;
		m_fTrendIntensity	= FLT_MAX;
		m_fTrendStability	= FLT_MAX;
		m_fMAWeighted		= FLT_MAX;
		m_fMAWeightedDif	= FLT_MAX;

		m_TimeKLine			= -1;
		m_TimeAction		= -1;
		m_fPrice			= FLT_MAX;		
		m_fMA5 				= FLT_MAX;
		m_fMA8				= FLT_MAX;
		m_fMA13				= FLT_MAX;
	};
	
}T_TrendIndexNode;

//要绘制的数据. 
class AFX_EXT_CLASS CNodeData
{
public:
	enum E_NodeDataFlag
	{
		KValueInvalid		= 0x00000001,			// 无效的取值
		KValueInhert		= 0x00000002,			// 延续上一笔数据
		KIdInvalid			= 0x00000004,			// 无效id
		KSelect				= 0x00000008,			// 选择
		KValueDivide		= 0x00000010,			// 分割点，该值后面的点不与该点连接，用于多日分时中
		KDrawNULL			= 0x00000020,			// 无效点，指标公式中指定的一些点, 这些点设为无效, 不画出来, 画线的时候也中断
	};

	enum E_NodeColorFlag
	{
		ENCFUseDefault = 0xff000000,	// 默认
		ENCFColorRise  = 0xff000001,	// 使用涨
		ENCFColorKeep  = 0xff000002,	// 使用平
		ENCFColorFall  = 0xff000003,	// 使用跌
	};

public:
	CNodeData();
	~CNodeData();

public:
	void	ClearValue( bool32 bClearID, bool32 bClearFlag);
	float   GetMax();
	float	GetMin();

	CNodeData(IN const CNodeData& NodeData);

public:
	uint32			m_iID;
	float			m_fOpen;
	float			m_fClose;
	float			m_fHigh;
	float			m_fLow;
	float			m_fAvg;
	float			m_fVolume;
	float			m_fAmount;
	float			m_fHold;

	uint32			m_iFlag;

	bool32			m_bClrValid;	// 颜色是否有效
	COLORREF		m_clrNode;		// node所使用的颜色，如果最高位为0x0，则代表使用该颜色
	// 0xff则代表loword是一个标志，0为以前的颜色，1为涨色，2为平盘，3为跌
private:
	void Set ( IN const CNodeData& NodeData);
};

class EXPORT_CLASS CIndexNodeList
{
public:
	CIndexNodeList();
	CIndexNodeList(const CIndexNodeList &src);

	~CIndexNodeList();

public:
	CIndexNodeList& operator=(const CIndexNodeList &src);

public:
	CString	m_StrExtraData;
	CStringArray m_aStrExtraData;
	void* m_pCurve;// （之前在主工程中定义时是CChartCurve类型）
	CArray<CNodeData, CNodeData&> m_aNodesFull;
	void	*m_pNodesShow;// （之前在主工程中定义时是CNodeSequence类型）
};

struct EXPORT_CLASS T_IndexParam
{
public:
	T_IndexParam();
	~T_IndexParam();

public:
	static int32	sIndexId;

public:
	TCHAR				strIndexName[256];					// 指标名称
	int32				id;									// 
	void				*pRegion;							// 所属Region （之前在主工程中定义时是CChartRegion类型）
	CFormularContent	*pContent;							// 指标公式字符串
	bool32				bMainIndex;							// 是否是主指标, false 的是叠加指标	
	bool32				bScreenAccroding;					// 是否跟屏幕当前显示的K线相关，显示K线变化需要重新计算
	///////////////////////////////////////////////////
	// 全部几条线
	CArray<CIndexNodeList, CIndexNodeList&> m_aIndexLines;

	// 当前屏幕显示的内容在原始数据中位置， 通过该内容生成NodesArray
	int32			m_iIndexShowPosInFullList;					// 当前屏幕显示的起始位置
	int32			m_iIndexShowCountInFullList;	
	bool32			m_bRight;								// 当前指标是否有权限
};

float	 AFX_EXT_API 	GetMax(float f1,float f2,float f3);
float	 AFX_EXT_API 	GetMin(float f1,float f2,float f3);
float	 AFX_EXT_API 	GetMiddle(float f1,float f2,float f3);

void	 AFX_EXT_API 	InitialTrendIndexParam();																// 初始化指标的参数
void	 AFX_EXT_API 	SetTrendIndexParam(float fStrongTrend,float fWeekTrend,float fOCTrend,float fStabilityTrend,int32 iMa5MoveNums,int32 iMa8MoveNums,int32 iMa13MoveNums,float fMa5Weight,float fMa8Weight,float fMa13Weight,int32 iCdt5KLineNums,float fCdt5Times); // 设置保存指标参数

// 取参数值
float AFX_EXT_API GetStrongTrend();																		// 强的趋势值
float AFX_EXT_API 		GetWeekTrend();																			// 弱的趋势值
float AFX_EXT_API 		GetOCTrend();																			// 开仓条件判断的趋势值
float AFX_EXT_API 		GetStabilityTrend();																	// 稳定性值
int32 AFX_EXT_API 		GetMA5MoveNums();																		// MA5 偏移
int32 AFX_EXT_API 		GetMA8MoveNums();																		// MA8 偏移	
int32 AFX_EXT_API 		GetMA13MoveNums();																		// MA13 偏移			
int32 AFX_EXT_API 		GetMaxMoveNums();																		// 最大偏移
float AFX_EXT_API 		GetMA5Weight();																			// MA5 权重
float AFX_EXT_API 		GetMA8Weight();																			// MA8 权重	
float AFX_EXT_API 		GetMA13Weight();																		// MA13 权重
int32 AFX_EXT_API 		GetCdt5KLineNums();																		// 条件5 中K线个数
float AFX_EXT_API 		GetCdt5Times();																			// 条件5 中的倍数
//		
bool32 AFX_EXT_API 		BeStability(const T_TrendIndexNode& Node,const T_TrendIndexNode& NodePre);				// 判断稳定性
E_TrendIntensity  AFX_EXT_API  BeStrong(const T_TrendIndexNode& Node);											// 判断强度	
//
float  AFX_EXT_API 		GetWeightedMAValue(float fMA5,float fMA8,float fMA13);									// 得到加权MA 值
CString  AFX_EXT_API 		GetActionPromptString(E_ActionPrompt eActionPrompt,COLORREF& clr);						// 得到字符串,顺便的一下颜色
CString	 AFX_EXT_API 	GetActionPromptString(E_ActionPrompt eActionPrompt);									// 得到操作提示字符串
CString	 AFX_EXT_API 	GetHoldStateString(E_HoldState eHoldState);												// 持仓状态字符串
CString	 AFX_EXT_API 	GetIntensityString(const T_TrendIndexNode& Node);										// 强度字符串
CString	 AFX_EXT_API 	GetStabilityString(const T_TrendIndexNode& Node,const T_TrendIndexNode& NodePre);		// 稳定性字符串
CString	 AFX_EXT_API 	GetTrendUpDownString(const T_TrendIndexNode& Node);										// 趋势字符串
CString  AFX_EXT_API   GetNodeSummary(const T_TrendIndexNode& Node,const T_TrendIndexNode& NodePre);			// 描述一个节点的字符串
//
float	 AFX_EXT_API 	CalcWeightMAWeightDif(int32 iPos, const CArray<T_TrendIndexNode, T_TrendIndexNode>& aNodeValues); // 算加权 MA 差值
//float		CalcIntensityVaule(float fMANow,float fMAPre,float fMAMax,float fMAMin);				// 算强度值(老的算法)
float	 AFX_EXT_API 	CalcIntensityVauleNew(int32 iPos, float fEMAWeightedDif, T_IndexOutArray* pIndexEMA, const CArray<CKLine,CKLine>& aKLines);				// 算强度值(新的)
float	 AFX_EXT_API 	CalcStabilityValue(float fIntensityNow,float fIntensityPre);							// 算稳定性值	
//
void	 AFX_EXT_API 	SortChoice(float* aValues,int32 iLength,OUT float& fMax,OUT float& fMin);				// 选择排序法
bool32	 AFX_EXT_API	GetBigCyleNode(IN const CTime& TimeNow,IN const CArray<T_TrendIndexNode,T_TrendIndexNode>& aBigCyleNodes,IN CTimeSpan TimeSpanBigCyle,OUT T_TrendIndexNode& NodeBigCyle); // 得到与本周起对应的大周期的节点

// 判断交易日:
bool32	 AFX_EXT_API 	BeLastTradeDayInaMounth(const CGmtTime& Time);												// 是否是当月最后交易日
bool32	 AFX_EXT_API 	BeLastTradeDayInaQuarter(const CGmtTime& Time);												// 是否是当季最后交易日
bool32	 AFX_EXT_API 	BeLastTradeDayInaYear(const CGmtTime& Time);												// 是否是当年最后交易日
	
// 统计函数
bool32	 AFX_EXT_API 	StatisticaTrendValues( IN int32 iKLineNums,													// 算多少根K 线
								   OUT int32& iKLineNumsReal,											// 实际算了多少根
								   IN const CArray<T_TrendIndexNode,T_TrendIndexNode>& aTrendValuesOUT, // 传入指标节点序列
								   OUT int32& iTradeTimes,												// 总交易次数		
								   OUT int32& iGoodTrades,												// 盈利交易
								   OUT int32& iBadTrades,											    // 亏损交易	
								   OUT float& fAccuracyRate,											// 准确率
								   OUT float& fProfitability);

bool32	 AFX_EXT_API 	StatisticaTrendValues2( IN const CArray<T_TrendIndexNode,T_TrendIndexNode>& aTrendValuesOUT, // 传入指标节点序列
								    OUT int32& iTradeTimes,												// 总交易次数		
								    OUT int32& iGoodTrades,												// 盈利交易
								    OUT int32& iBadTrades,											    // 亏损交易	
								    OUT float& fAccuracyRate,											// 准确率
								    OUT float& fProfitability);
#endif