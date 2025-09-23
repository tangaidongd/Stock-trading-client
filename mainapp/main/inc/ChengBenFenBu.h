#ifndef _CHENGBENFENBU_H_
#define _CHENGBENFENBU_H_

#include <vector>
#include <map>


#define CBFB_MAX_TRADERATERADIO (10.0)		// ˥��ϵ�� 0.1 - 10
#define CBFB_MIN_TRADERATERADIO (0.1)

#define CBFB_MIN_DIVIDE			(400)		// ���Էֳɶ��ٿ̶�
#define CBFB_MAX_DIVIDE			(800)
#define CBFB_DEFAULT_DIVIDE		((CBFB_MAX_DIVIDE+CBFB_MIN_DIVIDE)/2)

using std::vector;
using std::map;

enum E_ChengBenFenBuSuanFa
{
	ECBFB_PingJun = 0,		// ƽ���㷨
	ECBFB_SanJiao,			// �����㷨- δʵ��
};

struct T_ChengBenFenBuCalcNode
{
	int32 iPixel;	// ����
	float fPrice;	// �۸�
	float fVol;		// ��Ӧ�ĳɽ���
	
	T_ChengBenFenBuCalcNode()
	{
		iPixel = 0;
		fPrice = fVol = 0.0;
	}
};

struct T_ChengBenFenBuCalcParam
{
	E_ChengBenFenBuSuanFa eCBFBSuanFa;	// �㷨
	float   fTradeRateRatio;			// �����ʷŴ�ϵ�� - ��ʷ����˥��ϵ��
	float	fDefaultTradeRate;			// ȱʡ�Ļ�����
	UINT	m_uDivide;					// �ֳɶ��ٷ�, 
};

//typedef CArray<T_ChengBenFenBuCalcNode, const T_ChengBenFenBuCalcNode &> ChengBenFenBuNodeArray;
typedef vector<T_ChengBenFenBuCalcNode> ChengBenFenBuNodeArray;
typedef map<int32, ChengBenFenBuNodeArray >	ChengBenFenBuNodeMap;	// ��Ҫ���㱣�������ڽڵ� 0�������һ���ڵ�, 1�������ڶ�������
//typedef CArray<ChengBenFenBuNodeArray, const ChengBenFenBuNodeArray &> ChengBenFenBuNodeArrayArray;
typedef vector<ChengBenFenBuNodeArray >	ChengBenFenBuNodeArrayArray;	// ÿһ��ļ�����
typedef vector<ChengBenFenBuNodeMap::key_type>					ChengBenFenBuCycleArray;

struct T_ChengBenFenBuResult
{
	ChengBenFenBuNodeArray aResultNodes;	// ��������ս��

	struct T_PreVolSum
	{
		int32	iCycle;	// ����
		float   fPreVolSum;	// ����ǰ�ɱ�����
	};
	vector<T_PreVolSum> aPreVolSums;	// �������ڵ�ǰ�ɱ�����ͳ��
};

 // ��ͨ�� - ��
bool32 CalcChengBenFenBuRedY(const CKLine *pKline, int32 iKlineCount, float fLiuTongGuBen, const ChengBenFenBuCycleArray &aCycles, const T_ChengBenFenBuCalcParam *pCalcParam, OUT ChengBenFenBuNodeMap &mapNodes);
bool32 CalcChengBenFenBuBlueG(const CKLine *pKline, int32 iKlineCount, float fLiuTongGuBen, const ChengBenFenBuCycleArray &aCycles, const T_ChengBenFenBuCalcParam *pCalcParam, OUT ChengBenFenBuNodeMap &mapNodes);

bool32 CalcChengBenFenBu2(const CKLine *pKline, int32 iKlineCount, float fLiuTongGuBen, const ChengBenFenBuCycleArray &aCycles, const T_ChengBenFenBuCalcParam *pCalcParam, OUT T_ChengBenFenBuResult &ResultNode);

#endif //_CHENGBENFENBU_H_