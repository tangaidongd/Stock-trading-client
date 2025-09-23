#ifndef _CHENGBENFENBU_H_
#define _CHENGBENFENBU_H_

#include <vector>
#include <map>


#define CBFB_MAX_TRADERATERADIO (10.0)		// 衰减系数 0.1 - 10
#define CBFB_MIN_TRADERATERADIO (0.1)

#define CBFB_MIN_DIVIDE			(400)		// 可以分成多少刻度
#define CBFB_MAX_DIVIDE			(800)
#define CBFB_DEFAULT_DIVIDE		((CBFB_MAX_DIVIDE+CBFB_MIN_DIVIDE)/2)

using std::vector;
using std::map;

enum E_ChengBenFenBuSuanFa
{
	ECBFB_PingJun = 0,		// 平均算法
	ECBFB_SanJiao,			// 三角算法- 未实现
};

struct T_ChengBenFenBuCalcNode
{
	int32 iPixel;	// 像素
	float fPrice;	// 价格
	float fVol;		// 对应的成交量
	
	T_ChengBenFenBuCalcNode()
	{
		iPixel = 0;
		fPrice = fVol = 0.0;
	}
};

struct T_ChengBenFenBuCalcParam
{
	E_ChengBenFenBuSuanFa eCBFBSuanFa;	// 算法
	float   fTradeRateRatio;			// 换手率放大系数 - 历史换手衰减系数
	float	fDefaultTradeRate;			// 缺省的换手率
	UINT	m_uDivide;					// 分成多少份, 
};

//typedef CArray<T_ChengBenFenBuCalcNode, const T_ChengBenFenBuCalcNode &> ChengBenFenBuNodeArray;
typedef vector<T_ChengBenFenBuCalcNode> ChengBenFenBuNodeArray;
typedef map<int32, ChengBenFenBuNodeArray >	ChengBenFenBuNodeMap;	// 需要计算保留的周期节点 0代表最后一个节点, 1代表倒数第二个周期
//typedef CArray<ChengBenFenBuNodeArray, const ChengBenFenBuNodeArray &> ChengBenFenBuNodeArrayArray;
typedef vector<ChengBenFenBuNodeArray >	ChengBenFenBuNodeArrayArray;	// 每一天的计算结果
typedef vector<ChengBenFenBuNodeMap::key_type>					ChengBenFenBuCycleArray;

struct T_ChengBenFenBuResult
{
	ChengBenFenBuNodeArray aResultNodes;	// 计算的最终结果

	struct T_PreVolSum
	{
		int32	iCycle;	// 周期
		float   fPreVolSum;	// 周期前成本总量
	};
	vector<T_PreVolSum> aPreVolSums;	// 若干周期的前成本总量统计
};

 // 流通股 - 手
bool32 CalcChengBenFenBuRedY(const CKLine *pKline, int32 iKlineCount, float fLiuTongGuBen, const ChengBenFenBuCycleArray &aCycles, const T_ChengBenFenBuCalcParam *pCalcParam, OUT ChengBenFenBuNodeMap &mapNodes);
bool32 CalcChengBenFenBuBlueG(const CKLine *pKline, int32 iKlineCount, float fLiuTongGuBen, const ChengBenFenBuCycleArray &aCycles, const T_ChengBenFenBuCalcParam *pCalcParam, OUT ChengBenFenBuNodeMap &mapNodes);

bool32 CalcChengBenFenBu2(const CKLine *pKline, int32 iKlineCount, float fLiuTongGuBen, const ChengBenFenBuCycleArray &aCycles, const T_ChengBenFenBuCalcParam *pCalcParam, OUT T_ChengBenFenBuResult &ResultNode);

#endif //_CHENGBENFENBU_H_