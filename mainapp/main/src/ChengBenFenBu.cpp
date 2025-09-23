#include "StdAfx.h"
#include "MerchManager.h"

#include <cmath>
#include <float.h>

#include "ChengBenFenBu.h"
#include "Mmsystem.h"

#include <algorithm>
using namespace std;


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
bool32 CalcChengBenFenBuRedY(const CKLine *pKline, int32 iKlineCount, float fLiuTongGuBenOrg, const ChengBenFenBuCycleArray &aCycles, const T_ChengBenFenBuCalcParam *pCalcParam, OUT ChengBenFenBuNodeMap &mapNodes) // 流通股 - 手
{
#ifdef _DEBUG
	DWORD dwTimeStart = timeGetTime();
#endif
	mapNodes.clear(); // 清空结果

	if ( NULL == pKline )
	{
		ASSERT( 0 );
		return false;
	}

	float fPricePerPixel = 0.0f;
	E_ChengBenFenBuSuanFa eCBFBSuanFa = ECBFB_PingJun;	// 暂时不处理
	float fTradeRateRatio = 1.0f;
	float fDefaultTradeRate = 0.0f;
	UINT uDivide = CBFB_DEFAULT_DIVIDE;
	if ( pCalcParam != NULL )
	{
		eCBFBSuanFa = pCalcParam->eCBFBSuanFa;
		fTradeRateRatio = pCalcParam->fTradeRateRatio;
		fDefaultTradeRate = pCalcParam->fDefaultTradeRate;
		uDivide = pCalcParam->m_uDivide;

		if ( eCBFBSuanFa != ECBFB_PingJun && eCBFBSuanFa != ECBFB_SanJiao )
		{
			ASSERT( 0 );
			return false;
		}

		if ( fTradeRateRatio < CBFB_MIN_TRADERATERADIO || fTradeRateRatio > CBFB_MAX_TRADERATERADIO )
		{
			ASSERT( 0 );
			return false;
		}

		if ( uDivide < CBFB_MIN_DIVIDE || uDivide > CBFB_MAX_DIVIDE )
		{
			ASSERT( 0 );
			return false;
		}
	}

	int32 i = 0;
	float fPriceMin, fPriceMax;
	fPriceMin = FLT_MAX;
	fPriceMax = -FLT_MAX;
	float fLiuTongGuBen = fLiuTongGuBenOrg;
	if ( fLiuTongGuBen <= 1.0 && (fDefaultTradeRate <= 0.0 || fDefaultTradeRate >= 1.0) )
	{
		fLiuTongGuBen = 1e-6;
		for ( int32 i=0; i < iKlineCount ; i++ )
		{
			const CKLine &kline = pKline[i];
			fLiuTongGuBen += kline.m_fVolume;	// 所有成交量之和
			
			if ( kline.m_fPriceHigh > fPriceMax )
			{
				fPriceMax = kline.m_fPriceHigh;
			}
			if ( kline.m_fPriceLow < fPriceMin )
			{
				fPriceMin = kline.m_fPriceLow;
			}
		}
	}
	else 
	{
		for ( int32 i=0; i < iKlineCount ; i++ )
		{
			const CKLine &kline = pKline[i];
			
			if ( kline.m_fPriceHigh > fPriceMax )
			{
				fPriceMax = kline.m_fPriceHigh;
			}
			if ( kline.m_fPriceLow < fPriceMin )
			{
				fPriceMin = kline.m_fPriceLow;
			}
		}
	}
	if ( fLiuTongGuBen <= 1.0 )
	{
		fLiuTongGuBen = 1e-6;
	}

	if ( (fLiuTongGuBen <= 1.0 && (fDefaultTradeRate <= 0.0 || fDefaultTradeRate >= 1.0)) )
	{
		return false;	// 无法计算下去
	}


	
	// 排序其它的要求
	// 其它的节点都是xx日之前的
	ChengBenFenBuCycleArray aSortKeys;
	{
		ChengBenFenBuCycleArray aTmp = aCycles;
		aTmp.push_back(0);	// 总加入一个0
		sort(aTmp.begin(), aTmp.end());
		// 清除重复多余
		int32 i = 0;
		for ( i=0; i < aTmp.size()-1 ; i++ )
		{
			if ( aTmp.at(i) != aTmp.at(i+1) )
			{
				aSortKeys.push_back(aTmp.at(i));
			}
		}
		aSortKeys.push_back(aTmp.back());

		for ( i = 0; i < aSortKeys.size() ; i++  )
		{
			mapNodes[aSortKeys[i]];	// 插入默认值
		}
		
		ASSERT( aSortKeys[0]==0 && (aSortKeys.size() == 1 || aSortKeys[0] < aSortKeys[1]) );
	}
	// 计算结果总会插入0
	ChengBenFenBuNodeMap::iterator it = mapNodes.find(0);
	if ( it == mapNodes.end() )
	{
		ASSERT( 0 );
		return false;
	}
	ChengBenFenBuNodeArray &aNodes = it->second;	// 别名

	aNodes.clear();
	fPricePerPixel = (fPriceMax-fPriceMin)/uDivide;		// 每等分的价格
	aNodes.insert(aNodes.end(), uDivide, T_ChengBenFenBuCalcNode());	// 赋值默认
	
	i = 0;
	for ( ChengBenFenBuNodeArray::iterator itArray=aNodes.begin(); itArray != aNodes.end() ; itArray++ )
	{
		itArray->iPixel = i;
		itArray->fPrice = fPriceMin + fPricePerPixel *i;
		i++;
	}

	// http://hi.baidu.com/lweml/blog/item/e9a3a764ce546cfff736541c.html
	// a)上市每一天的成本分布图就是当日成本，即最低价到最高价间的一个矩形。	
	// b)其后每一天的成本分布图满足Y=（1-A）*Y’+A*B，A表示当日换手，B表示当日成本，Y、Y’分别表示当日和上一日的成本分布，注意，此处BYY’均表示一个分布情况，而不是一个数值。
	// 矫正:  A*B不需要乘以A这个数，因为这个数实际其它软件并没有乘
	int32 iIndex = 0;		// ~~
	if ( iKlineCount > 0 )
	{
		// 取第一个数据点 - 第一个成本点这样取的话就太高了, 取消此
// 		while ( iIndex < iKlineCount )
// 		{
// 			const CKLine &kline = pKline[iIndex++];	// !!
// 
// 			const int32 iEndPos    = int32((kline.m_fPriceHigh - fPriceMin)/ fPricePerPixel);
// 			const int32 iStartPos = int32((kline.m_fPriceLow - fPriceMin)/fPricePerPixel);
// 			const float fVolPerPixel = kline.m_fVolume / (iEndPos-iStartPos+1);
// 			ASSERT( !_isnan(fVolPerPixel) && _finite(fVolPerPixel) && fVolPerPixel >= 0.0 );
// 			if ( _isnan(fVolPerPixel) || !_finite(fVolPerPixel) || fVolPerPixel < 0.0 )
// 			{
// 				continue;	// 非法值 必须跳过
// 			}
// 			//ASSERT( aNodes.size() == uDivide );
// 			for ( int32 iNode = iStartPos; iNode < aNodes.size() && iNode <= iEndPos ; iNode++ )
// 			{
// 				aNodes.at(iNode).fVol += fVolPerPixel;	// 赋予第一次计算结果，将成交量平均到每个单位坐标上
// 			}
// 			break;	// 只取第一个有效值
// 		}
	}

	
	
	// 后续数据点 Y = (1-A)Y' + B*A
	
	// 从距离最新周期远的到近的
	for ( vector<ChengBenFenBuNodeMap::key_type>::reverse_iterator itId = aSortKeys.rbegin(); itId != aSortKeys.rend() ; itId++ )
	{
		const int32 iCalcCount = iKlineCount - *itId;	// 数字id代表距离最新周期多少周期

		if ( iCalcCount < iIndex )
		{
			continue; // 不能计算这个周期
		}

		// 平均算法与三角形算法
		// 三角形 均价线最长 往高，低价减少
		if ( ECBFB_SanJiao == eCBFBSuanFa )
		{
			for ( ; iIndex < iCalcCount ; iIndex++ )
			{
				const CKLine &kline = pKline[iIndex]; 
				
				float fRate = fDefaultTradeRate;
				if ( fLiuTongGuBen > 1.0 )
				{
					fRate = kline.m_fVolume / fLiuTongGuBen; // 换手率
				}
				
				fRate = fRate * fTradeRateRatio;	// 乘以衰减系数
				
				//ASSERT( fRate < 1.0 && fRate > 0.0 );
				if ( fRate <= 0.0 )
				{
					fRate = 0.0001f;
					//continue;	// 错误情况
				}
				else if ( fRate >= 1.0 )
				{
					fRate = 0.999f;		// 纠正错误？？
				}
				
				// 三角形算法
				//const int32 iCount    = int32((kline.m_fPriceHigh - kline.m_fPriceLow)/ fPricePerPixel) +1;
				const int32 iEndPos    = int32((kline.m_fPriceHigh - fPriceMin)/ fPricePerPixel);
				const int32 iStartPos = int32((kline.m_fPriceLow - fPriceMin)/fPricePerPixel);
				const float fVolPerPixel = kline.m_fVolume / (iEndPos-iStartPos+1);
				int32 iLow2AvgPos = (iEndPos+iStartPos)/2;	// 中间价位置
				if ( !(fVolPerPixel > 0.0) )
				{
					continue;	// 非法值跳过
				}
				// 以avg为平分点，上半部分按照[1->n]累加分摊1/2的成交量 下半部分同
				const int32 iLow2AvgCount = (iLow2AvgPos-iStartPos+1);
				const int32 iHigh2AvgCount = (iEndPos-iLow2AvgPos+1);
				const float fLowPerPixel = kline.m_fVolume / (iLow2AvgCount*(iLow2AvgCount+1));
				const float fHighPerPixel = kline.m_fVolume / (iHigh2AvgCount*(iHigh2AvgCount+1));
				
				//ASSERT( aNodes.size() == uDivide );
				const float fRate2 = 1.0f - fRate;
				int32 iNode;
				for ( iNode=0; iNode < aNodes.size() ; iNode++ )
				{
					T_ChengBenFenBuCalcNode &node = aNodes.at(iNode);	// 上一次计算的结果
					node.fVol = (fRate2)*node.fVol;		// 全部衰减
				}
				for ( iNode=iStartPos; iNode <= iLow2AvgPos && iNode < aNodes.size() ; iNode++ )
				{
					// 下半部分, 含均价
					T_ChengBenFenBuCalcNode &node = aNodes.at(iNode);	// 上一次计算的结果
					node.fVol += /*fRate**/fLowPerPixel*(iNode-iStartPos+1);		// 衰减
				}
				for ( iNode=iLow2AvgPos; iNode<=iEndPos&&iNode<aNodes.size() ; iNode++ )
				{
					// 上半部分，含均价
					T_ChengBenFenBuCalcNode &node = aNodes.at(iNode);	// 上一次计算的结果
					node.fVol += /*fRate**/fHighPerPixel*(iEndPos-iNode+1);		// 衰减
				}
				
				// 对前面已经处理了的周期进行平滑处理
				{
					// 此处应该可以移植运算完成后平滑，TODO
					for ( vector<ChengBenFenBuNodeMap::key_type>::reverse_iterator itPreId = aSortKeys.rbegin(); itPreId != itId ; itPreId++ )
					{
						if ( iKlineCount - *itPreId <= 0 )
						{
							continue;	// 这些是不能处理的
						}
						//ASSERT( *itPreId != it->first );
						
						ChengBenFenBuNodeMap::iterator itPre = mapNodes.find(*itPreId);
						//ASSERT( aNodes.size() == itPre->second.size() );
						for ( iNode = 0; iNode < itPre->second.size() ; iNode++ )	// 计算每一个单位坐标
						{
							T_ChengBenFenBuCalcNode &node = itPre->second.at(iNode);	// 上一次计算的结果
							// 全部都是没有的
							node.fVol  = (fRate2)*node.fVol;	// 是不是这个意思？
						}
					}
				}
			}
		}
		else
		{
			for ( ; iIndex < iCalcCount ; iIndex++ )
			{
				const CKLine &kline = pKline[iIndex]; 
				
				float fRate = fDefaultTradeRate;
				if ( fLiuTongGuBen > 1.0 )
				{
					fRate = kline.m_fVolume / fLiuTongGuBen; // 换手率
				}
				
				fRate = fRate * fTradeRateRatio;	// 乘以衰减系数
				const float fRate2 = 1.0-fRate;
				
				//ASSERT( fRate < 1.0 && fRate > 0.0 );
				if ( fRate <= 0.0 )
				{
					fRate = 0.0001f;
					//continue;	// 错误情况
				}
				else if ( fRate >= 1.0 )
				{
					fRate = 0.999f;		// 纠正错误？？
				}
				
				int32 iEndPos    = int32((kline.m_fPriceHigh - fPriceMin)/ fPricePerPixel);
				const int32 iStartPos = int32((kline.m_fPriceLow - fPriceMin)/fPricePerPixel);
				int32 iNodeSize = aNodes.size() - 1;
				iEndPos = min(iEndPos, iNodeSize);
				const float fVolPerPixel = kline.m_fVolume / (iEndPos-iStartPos+1);
				if ( !(fVolPerPixel > 0.0) )
				{
					continue;	// 非法值跳过
				}
				
				//ASSERT( aNodes.size() == uDivide );
				int32 iNode;
				for ( iNode = 0; iNode < aNodes.size() ; iNode++ )	// 计算每一个单位坐标
				{
					// 所有值衰减
					T_ChengBenFenBuCalcNode &node = aNodes.at(iNode);	// 上一次计算的结果
					// 有些点是有值的，有些点是没有值的
					node.fVol  = fRate2*node.fVol;	// 是不是这个意思？
				}
				for ( iNode=iStartPos; iNode <= iEndPos && iNode < aNodes.size() ; iNode++ )
				{
					// 当日值累加
					T_ChengBenFenBuCalcNode &node = aNodes.at(iNode);	// 上一次计算的结果
					node.fVol  +=  fVolPerPixel /** fRate*/;	// 是不是这个意思？, 不需要乘以这个数，因为这个数实际其它软件并没有乘
				}
				
				// 对前面已经处理了的周期进行平滑处理
				{
					// 此处应该可以移植运算完成后平滑，TODO
					for ( vector<ChengBenFenBuNodeMap::key_type>::reverse_iterator itPreId = aSortKeys.rbegin(); itPreId != itId ; itPreId++ )
					{
						if ( iKlineCount - *itPreId <= 0 )
						{
							continue;	// 这些是不能处理的
						}
						//ASSERT( *itPreId != it->first );
						
						ChengBenFenBuNodeMap::iterator itPre = mapNodes.find(*itPreId);
						//ASSERT( aNodes.size() == itPre->second.size() );
						for ( iNode = 0; iNode < itPre->second.size() ; iNode++ )	// 计算每一个单位坐标
						{
							T_ChengBenFenBuCalcNode &node = itPre->second.at(iNode);	// 上一次计算的结果
							// 全部都是没有的
							node.fVol  = fRate2*node.fVol;	// 是不是这个意思？
						}
					}
				}
			}
		}

		if ( *itId != it->first )
		{
			mapNodes[ *itId ] = aNodes;	// 这个这个时候状态拷贝下来就是距离n之前的结果
		}
	}

// 	{
// 		// 输出到调试文件
// 		char buf[200];
// 		float fVol = 0.0;
// 		HANDLE hFile = CreateFile(_T("d:\\cbfb.nodes"), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);
// 		DWORD dw = 0;
// 		for ( int32 iNode = 0; iNode < aNodes.size() ; iNode++ )	// 计算每一个单位坐标
// 		{
// 			T_ChengBenFenBuCalcNode &node = aNodes.at(iNode);	// 上一次计算的结果
// 			// 全部都是没有的
// 			_snprintf(buf, sizeof(buf), "%d - %0.3f - %f\r\n", node.iPixel, node.fPrice, node.fVol);
// 			WriteFile(hFile, buf, strlen(buf), &dw, NULL);
// 			fVol += node.fVol;
// 		}
// 		_snprintf(buf, sizeof(buf), "共: %f\r\n", fVol);
// 		WriteFile(hFile, buf, strlen(buf), &dw, NULL);
// 		DEL_HANDLE(hFile);
// 	}

#ifdef _DEBUG
	dwTimeStart = timeGetTime() - dwTimeStart;
	TRACE(_T("成本分布计算(红黄,方法:%d [%d*%d]): %d ms\r\n"), eCBFBSuanFa, iKlineCount, aNodes.size(), dwTimeStart);
#endif
	

	return true;
}

bool32 CalcChengBenFenBuBlueG( const CKLine *pKline, int32 iKlineCount, float fLiuTongGuBenOrg, const ChengBenFenBuCycleArray &aCycles, const T_ChengBenFenBuCalcParam *pCalcParam, OUT ChengBenFenBuNodeMap &mapNodes )
{
#ifdef _DEBUG
	DWORD dwTimeStart = timeGetTime();
#endif
	

	int32 iNodeSize = 0;
	if ( CalcChengBenFenBuRedY(pKline, iKlineCount, fLiuTongGuBenOrg, aCycles, pCalcParam, mapNodes) )
	{
		// 现在的结果是xx日外的数据，0日外的数据为所有日期内数据
		// 蓝绿图中0还是所有日期数据不变，其它数据与红黄互补
		ChengBenFenBuNodeMap::iterator itTotal = mapNodes.find(0);
		if ( itTotal == mapNodes.end() )
		{
			ASSERT( 0 );
			return false;
		}
		const ChengBenFenBuNodeArray &aTotalNodes = itTotal->second;	// 总数据
		iNodeSize = aTotalNodes.size();
		for ( ChengBenFenBuNodeMap::iterator itCalc=mapNodes.begin()
			; itCalc != mapNodes.end()
			; ++itCalc )
		{
			if ( itCalc == itTotal )
			{
				continue;	// 跳过总数据
			}

			ASSERT( itCalc->second.size()==0 || itCalc->second.size() == aTotalNodes.size() );
			for ( int32 iNode=0; iNode < itCalc->second.size() ; iNode++ )
			{
				T_ChengBenFenBuCalcNode &node = itCalc->second.at(iNode);
				node.fVol = aTotalNodes[iNode].fVol - node.fVol;
			}
		}
	}

#ifdef _DEBUG
	dwTimeStart = timeGetTime() - dwTimeStart;
	TRACE(_T("成本分布计算(蓝绿[%d*%d]): %d ms\r\n"), iKlineCount, iNodeSize, dwTimeStart);
#endif
	

	return true;
}


bool32 CalcChengBenFenBuBlueG2( const CKLine *pKline, int32 iKlineCount, float fLiuTongGuBenOrg, const ChengBenFenBuCycleArray &aCycles, const T_ChengBenFenBuCalcParam *pCalcParam, OUT ChengBenFenBuNodeMap &mapNodes )
{
#ifdef _DEBUG
	DWORD dwTimeStart = timeGetTime();
#endif
	mapNodes.clear(); // 清空结果

	if ( NULL == pKline )
	{
		ASSERT( 0 );
		return false;
	}

	float fPricePerPixel = 0.0f;
	E_ChengBenFenBuSuanFa eCBFBSuanFa = ECBFB_PingJun;	// 暂时不处理
	float fTradeRateRatio = 1.0f;
	float fDefaultTradeRate = 0.0f;
	UINT uDivide = CBFB_DEFAULT_DIVIDE;
	if ( pCalcParam != NULL )
	{
		eCBFBSuanFa = pCalcParam->eCBFBSuanFa;
		fTradeRateRatio = pCalcParam->fTradeRateRatio;
		fDefaultTradeRate = pCalcParam->fDefaultTradeRate;
		uDivide = pCalcParam->m_uDivide;

		if ( eCBFBSuanFa != ECBFB_PingJun && eCBFBSuanFa != ECBFB_SanJiao )
		{
			ASSERT( 0 );
			return false;
		}

		if ( fTradeRateRatio < CBFB_MIN_TRADERATERADIO || fTradeRateRatio > CBFB_MAX_TRADERATERADIO )
		{
			ASSERT( 0 );
			return false;
		}

		if ( uDivide < CBFB_MIN_DIVIDE || uDivide > CBFB_MAX_DIVIDE )
		{
			ASSERT( 0 );
			return false;
		}
	}

	int32 i = 0;
	float fPriceMin, fPriceMax;
	fPriceMin = FLT_MAX;
	fPriceMax = -FLT_MAX;
	float fLiuTongGuBen = fLiuTongGuBenOrg;
	if ( fLiuTongGuBen <= 1.0 && (fDefaultTradeRate <= 0.0 || fDefaultTradeRate >= 1.0) )
	{
		fLiuTongGuBen = 1e-6;
		for ( int32 i=0; i < iKlineCount ; i++ )
		{
			const CKLine &kline = pKline[i];
			fLiuTongGuBen += kline.m_fVolume;	// 所有成交量之和
			
			if ( kline.m_fPriceHigh > fPriceMax )
			{
				fPriceMax = kline.m_fPriceHigh;
			}
			if ( kline.m_fPriceLow < fPriceMin )
			{
				fPriceMin = kline.m_fPriceLow;
			}
		}
	}
	else 
	{
		for ( int32 i=0; i < iKlineCount ; i++ )
		{
			const CKLine &kline = pKline[i];
			
			if ( kline.m_fPriceHigh > fPriceMax )
			{
				fPriceMax = kline.m_fPriceHigh;
			}
			if ( kline.m_fPriceLow < fPriceMin )
			{
				fPriceMin = kline.m_fPriceLow;
			}
		}
	}
	if ( fLiuTongGuBen <= 1.0 )
	{
		fLiuTongGuBen = 1e-6;
	}

	if ( (fLiuTongGuBen <= 1.0 && (fDefaultTradeRate <= 0.0 || fDefaultTradeRate >= 1.0)) )
	{
		return false;	// 无法计算下去
	}

	

	
	// 排序其它的要求
	// 其它的节点都是xx日之前的
	ChengBenFenBuCycleArray aSortKeys;
	{
		ChengBenFenBuCycleArray aTmp = aCycles;
		aTmp.push_back(0);	// 总加入一个0
		sort(aTmp.begin(), aTmp.end());
		// 清除重复多余
		int32 i = 0;
		for ( i=0; i < aTmp.size()-1 ; i++ )
		{
			if ( aTmp.at(i) != aTmp.at(i+1) )
			{
				aSortKeys.push_back(aTmp.at(i));
			}
		}
		aSortKeys.push_back(aTmp.back());

		for ( i = 0; i < aSortKeys.size() ; i++  )
		{
			mapNodes[aSortKeys[i]];	// 插入默认值
		}
		
		ASSERT( aSortKeys[0]==0 && (aSortKeys.size() == 1 || aSortKeys[0] < aSortKeys[1]) );
	}
	// 计算结果总会插入0
	ChengBenFenBuNodeMap::iterator it = mapNodes.find(0);
	if ( it == mapNodes.end() )
	{
		ASSERT( 0 );
		return false;
	}
	ChengBenFenBuNodeArray &aNodes = it->second;	// 别名

	aNodes.clear();
	fPricePerPixel = (fPriceMax-fPriceMin)/uDivide;		// 每等分的价格
	aNodes.insert(aNodes.end(), uDivide, T_ChengBenFenBuCalcNode());	// 赋值默认
	
	i = 0;
	for ( ChengBenFenBuNodeArray::iterator itArray=aNodes.begin(); itArray != aNodes.end() ; itArray++ )
	{
		itArray->iPixel = i;
		itArray->fPrice = fPriceMin + fPricePerPixel *i;
		i++;
	}

	// a)上市每一天的成本分布图就是当日成本，即最低价到最高价间的一个矩形。	
	// b)其后每一天的成本分布图满足Y=（1-A）*Y’+A*B，A表示当日换手，B表示当日成本，Y、Y’分别表示当日和上一日的成本分布，注意，此处BYY’均表示一个分布情况，而不是一个数值。
	int32 iIndex = 0;		// ~~
	if ( iKlineCount > 0 )
	{
		// 取第一个数据点, 第一个数据点应该不是这么取的，放弃该取法
// 		while ( iIndex < iKlineCount )
// 		{
// 			const CKLine &kline = pKline[iIndex++];	// !!
// 
// 			const int32 iEndPos    = int32((kline.m_fPriceHigh - fPriceMin)/ fPricePerPixel);
// 			const int32 iStartPos = int32((kline.m_fPriceLow - fPriceMin)/fPricePerPixel);
// 			const float fVolPerPixel = kline.m_fVolume / (iEndPos-iStartPos+1);
// 			ASSERT( !_isnan(fVolPerPixel) && _finite(fVolPerPixel) && fVolPerPixel >= 0.0 );
// 			if ( _isnan(fVolPerPixel) || !_finite(fVolPerPixel) || fVolPerPixel < 0.0 )
// 			{
// 				continue;	// 非法值 必须跳过
// 			}
// 			//ASSERT( aNodes.size() == uDivide );
// 			for ( int32 iNode = iStartPos; iNode < aNodes.size() && iNode <= iEndPos ; iNode++ )
// 			{
// 				aNodes.at(iNode).fVol += fVolPerPixel;	// 赋予第一次计算结果，将成交量平均到每个单位坐标上
// 			}
// 			break;	// 只取第一个有效值
// 		}
	}

	
	
	// 后续数据点 Y = (1-A)Y' + B*A
	
	// 从距离最新周期远的到近的
	for ( vector<ChengBenFenBuNodeMap::key_type>::reverse_iterator itId = aSortKeys.rbegin(); itId != aSortKeys.rend() ; itId++ )
	{
		const int32 iCalcCount = iKlineCount - *itId;	// 数字id代表距离最新周期多少周期

		if ( iCalcCount < iIndex )
		{
			continue; // 不能计算这个周期
		}

		// 平均算法与三角形算法
		// 三角形 均价线最长 往高，低价减少
		if ( ECBFB_SanJiao == eCBFBSuanFa )
		{
			for ( ; iIndex < iCalcCount ; iIndex++ )
			{
				const CKLine &kline = pKline[iIndex]; 
				
				float fRate = fDefaultTradeRate;
				if ( fLiuTongGuBen > 1.0 )
				{
					fRate = kline.m_fVolume / fLiuTongGuBen; // 换手率
				}
				
				fRate = fRate * fTradeRateRatio;	// 乘以衰减系数
				
				//ASSERT( fRate < 1.0 && fRate > 0.0 );
				if ( fRate <= 0.0 )
				{
					fRate = 0.0001f;
					//continue;	// 错误情况
				}
				else if ( fRate >= 1.0 )
				{
					fRate = 0.999f;		// 纠正错误？？
				}
				
				// 三角形算法
				//const int32 iCount    = int32((kline.m_fPriceHigh - kline.m_fPriceLow)/ fPricePerPixel) +1;
				const int32 iEndPos    = int32((kline.m_fPriceHigh - fPriceMin)/ fPricePerPixel);
				const int32 iStartPos = int32((kline.m_fPriceLow - fPriceMin)/fPricePerPixel);
				const float fVolPerPixel = kline.m_fVolume / (iEndPos-iStartPos+1);
				int32 iLow2AvgPos = int32((kline.m_fPriceAvg - fPriceMin)/ fPricePerPixel);	// 中间价位置
				if ( !(fVolPerPixel > 0.0) )
				{
					continue;	// 非法值跳过
				}
				// 以avg为平分点，上半部分按照[1->n]累加分摊1/2的成交量 下半部分同
				const int32 iLow2AvgCount = (iLow2AvgPos-iStartPos+1);
				const int32 iHigh2AvgCount = (iEndPos-iLow2AvgPos+1);
				const float fLowPerPixel = kline.m_fVolume / (iLow2AvgCount*(iLow2AvgCount+1));
				const float fHighPerPixel = kline.m_fVolume / (iHigh2AvgCount*(iHigh2AvgCount+1));

				//ASSERT( aNodes.size() == uDivide );
				int32 iNode;
				for ( iNode = 0; iNode < aNodes.size() ; iNode++ )	// 计算每一个单位坐标
				{
					T_ChengBenFenBuCalcNode &node = aNodes.at(iNode);	// 上一次计算的结果
					// 有些点是有值的，有些点是没有值的, 对所有点进行历史衰减操作
					node.fVol  = (1.0-fRate)*node.fVol;	// 是不是这个意思？
				}
				for ( iNode=iStartPos; iNode <= iLow2AvgPos && iNode < aNodes.size() ; iNode++ )
				{
					// 下半部分, 含均价
					T_ChengBenFenBuCalcNode &node = aNodes.at(iNode);	// 上一次计算的结果
					node.fVol += /*fRate**/fLowPerPixel*(iNode-iStartPos+1);		// 加上今日值
				}
				for ( iNode=iLow2AvgPos; iNode<=iEndPos&&iNode<aNodes.size() ; iNode++ )
				{
					// 上半部分，含均价
					T_ChengBenFenBuCalcNode &node = aNodes.at(iNode);	// 上一次计算的结果
					node.fVol += /*fRate**/fHighPerPixel*(iEndPos-iNode+1);		// 加上今日值
				}
				
				// 蓝绿图处理
				// 对前面已经处理了的周期进行加值处理
				{
					// 此处应该可以移植运算完成后平滑，TODO
					for ( vector<ChengBenFenBuNodeMap::key_type>::reverse_iterator itPreId = aSortKeys.rbegin(); itPreId != itId ; itPreId++ )
					{
						if ( iKlineCount - *itPreId <= 0 )
						{
							continue;	// 这些是不能处理的
						}
						//ASSERT( *itPreId != it->first );
						
						ChengBenFenBuNodeMap::iterator itPre = mapNodes.find(*itPreId);
						//ASSERT( aNodes.size() == itPre->second.size() );

						for ( iNode = 0; iNode < itPre->second.size() ; iNode++ )	// 计算每一个单位坐标
						{
							T_ChengBenFenBuCalcNode &node = itPre->second.at(iNode);	// 上一次计算的结果
							// 有些点是有值的，有些点是没有值的, 对所有点进行历史衰减操作
							node.fVol  = (1.0-fRate)*node.fVol;	// 是不是这个意思？
						}
						for ( iNode=iStartPos; iNode <= iLow2AvgPos && iNode < itPre->second.size() ; iNode++ )
						{
							// 下半部分, 含均价
							T_ChengBenFenBuCalcNode &node = itPre->second.at(iNode);	// 上一次计算的结果
							node.fVol += /*fRate**/fLowPerPixel*(iNode-iStartPos+1);		// 加上今日值
						}
						for ( iNode=iLow2AvgPos; iNode<=iEndPos&&iNode<itPre->second.size() ; iNode++ )
						{
							// 上半部分，含均价
							T_ChengBenFenBuCalcNode &node = itPre->second.at(iNode);	// 上一次计算的结果
							node.fVol += /*fRate**/fHighPerPixel*(iEndPos-iNode+1);		// 加上今日值
						}
					}
				}
			}
		}
		else
		{
			for ( ; iIndex < iCalcCount ; iIndex++ )
			{
				const CKLine &kline = pKline[iIndex]; 
				
				float fRate = fDefaultTradeRate;
				if ( fLiuTongGuBen > 1.0 )
				{
					fRate = kline.m_fVolume / fLiuTongGuBen; // 换手率
				}
				
				fRate = fRate * fTradeRateRatio;	// 乘以衰减系数
				
				//ASSERT( fRate < 1.0 && fRate > 0.0 );
				if ( fRate <= 0.0 )
				{
					fRate = 0.0001f;
					//continue;	// 错误情况
				}
				else if ( fRate >= 1.0 )
				{
					fRate = 0.999f;		// 纠正错误？？
				}
				
				int32 iEndPos    = int32((kline.m_fPriceHigh - fPriceMin)/ fPricePerPixel);
				int32 iNodeSize  = aNodes.size();
				iEndPos = min(iEndPos, iNodeSize);
				const int32 iStartPos = int32((kline.m_fPriceLow - fPriceMin)/fPricePerPixel);
				const float fVolPerPixel = kline.m_fVolume / (iEndPos-iStartPos+1);
				if ( !(fVolPerPixel > 0.0) )
				{
					continue;	// 非法值跳过
				}
				
				//ASSERT( aNodes.size() == uDivide );
				int32 iNode;
				for ( iNode = 0; iNode < aNodes.size() ; iNode++ )	// 计算每一个单位坐标
				{
					T_ChengBenFenBuCalcNode &node = aNodes.at(iNode);	// 上一次计算的结果
					// 有些点是有值的，有些点是没有值的, 对所有点进行历史衰减操作
					node.fVol  = (1.0-fRate)*node.fVol;	// 是不是这个意思？
				}
				for ( iNode = iStartPos; iNode < aNodes.size() && iNode <= iEndPos ; iNode++ )	// 计算每一个单位坐标
				{
					T_ChengBenFenBuCalcNode &node = aNodes.at(iNode);	// 上一次计算的结果
					// 有些点是有值的，有些点是没有值的, 对有值的点进行今日成本堆积操作
					node.fVol  += fVolPerPixel * fRate;	// 是不是这个意思？
					ASSERT( node.fVol >= 0.0 );
				}
				
				// 蓝绿图处理
				// 对前面已经处理了的周期进行加值处理
				{
					// 此处应该可以移植运算完成后平滑，TODO
					for ( vector<ChengBenFenBuNodeMap::key_type>::reverse_iterator itPreId = aSortKeys.rbegin(); itPreId != itId ; itPreId++ )
					{
						if ( iKlineCount - *itPreId <= 0 )
						{
							continue;	// 这些是不能处理的
						}
						//ASSERT( *itPreId != it->first );
						
						ChengBenFenBuNodeMap::iterator itPre = mapNodes.find(*itPreId);
						//ASSERT( aNodes.size() == itPre->second.size() );
						
						for ( iNode = 0; iNode < itPre->second.size() ; iNode++ )	// 计算每一个单位坐标
						{
							T_ChengBenFenBuCalcNode &node = itPre->second.at(iNode);	// 上一次计算的结果
							// 有些点是有值的，有些点是没有值的, 对所有点进行历史衰减操作
							node.fVol  = (1.0-fRate)*node.fVol;	// 是不是这个意思？
						}
						for ( iNode = iStartPos; iNode < itPre->second.size() && iNode < iEndPos ; iNode++ )	// 计算每一个单位坐标
						{
							T_ChengBenFenBuCalcNode &node = itPre->second.at(iNode);	// 上一次计算的结果
							// 有些点是有值的，有些点是没有值的, 对有值的点进行今日成本堆积操作
							node.fVol  += fVolPerPixel * fRate;	// 是不是这个意思？
							ASSERT( node.fVol >= 0.0 );
						}
					}
				}
			}
		}

		if ( *itId != it->first )
		{
			//mapNodes[ *itId ] = aNodes;	// 这个这个时候状态拷贝下来就是距离n之前的结果
			// 此时对该id的赋予0值
			ChengBenFenBuNodeMap::iterator itPre = mapNodes.find(*itId);
			itPre->second = aNodes;
			for ( int32 iNode=0; iNode < itPre->second.size() ; iNode++ )
			{
				itPre->second.at(iNode).fVol = 0.0f;
			}
		}
	}

#ifdef _DEBUG
	dwTimeStart = timeGetTime() - dwTimeStart;
	TRACE(_T("成本分布计算(蓝绿方法:%d[%d*%d]): %d ms\r\n"), eCBFBSuanFa, iKlineCount, aNodes.size(), dwTimeStart);
#endif
	

	return true;
}

bool32 CalcChengBenFenBu2( const CKLine *pKline, int32 iKlineCount, float fLiuTongGuBenOrg, const ChengBenFenBuCycleArray &aCycles, const T_ChengBenFenBuCalcParam *pCalcParam, OUT T_ChengBenFenBuResult &ResultNode )
{
#ifdef _DEBUG
	DWORD dwTimeStart = timeGetTime();
#endif
	ResultNode.aResultNodes.clear(); // 清空结果
	ResultNode.aPreVolSums.clear();

	if ( NULL == pKline )
	{
		ASSERT( 0 );
		return false;
	}

	float fPricePerPixel = 0.0f;
	E_ChengBenFenBuSuanFa eCBFBSuanFa = ECBFB_PingJun;	// 暂时不处理
	float fTradeRateRatio = 1.0f;
	float fDefaultTradeRate = 0.0f;
	UINT uDivide = CBFB_DEFAULT_DIVIDE;
	if ( pCalcParam != NULL )
	{
		eCBFBSuanFa = pCalcParam->eCBFBSuanFa;
		fTradeRateRatio = pCalcParam->fTradeRateRatio;
		fDefaultTradeRate = pCalcParam->fDefaultTradeRate;
		uDivide = pCalcParam->m_uDivide;

		if ( eCBFBSuanFa != ECBFB_PingJun && eCBFBSuanFa != ECBFB_SanJiao )
		{
			ASSERT( 0 );
			return false;
		}

		if ( fTradeRateRatio < CBFB_MIN_TRADERATERADIO || fTradeRateRatio > CBFB_MAX_TRADERATERADIO )
		{
			ASSERT( 0 );
			return false;
		}

		if ( uDivide < CBFB_MIN_DIVIDE || uDivide > CBFB_MAX_DIVIDE )
		{
			ASSERT( 0 );
			return false;
		}
	}

	int32 i = 0;
	float fPriceMin, fPriceMax;
	fPriceMin = FLT_MAX;
	fPriceMax = -FLT_MAX;
	float fLiuTongGuBen = fLiuTongGuBenOrg;
	if ( fLiuTongGuBen <= 1.0 && (fDefaultTradeRate <= 0.0 || fDefaultTradeRate >= 1.0) )
	{
		fLiuTongGuBen = 1e-6;
		for ( i=0; i < iKlineCount ; i++ )
		{
			const CKLine &kline = pKline[i];
			fLiuTongGuBen += kline.m_fVolume;	// 所有成交量之和
			
			if ( kline.m_fPriceHigh > fPriceMax )
			{
				fPriceMax = kline.m_fPriceHigh;
			}
			if ( kline.m_fPriceLow < fPriceMin )
			{
				fPriceMin = kline.m_fPriceLow;
			}
		}
	}
	else 
	{
		for ( i=0; i < iKlineCount ; i++ )
		{
			const CKLine &kline = pKline[i];
			
			if ( kline.m_fPriceHigh > fPriceMax )
			{
				fPriceMax = kline.m_fPriceHigh;
			}
			if ( kline.m_fPriceLow < fPriceMin )
			{
				fPriceMin = kline.m_fPriceLow;
			}
		}
	}
	if ( fLiuTongGuBen <= 1.0 )
	{
		fLiuTongGuBen = 1e-6;
	}

	if ( (fLiuTongGuBen <= 1.0 && (fDefaultTradeRate <= 0.0 || fDefaultTradeRate >= 1.0)) )
	{
		return false;	// 无法计算下去
	}


	
	// 排序其它的要求
	// 其它的节点都是xx日之前的
	ChengBenFenBuCycleArray aSortKeys;
	{
		ChengBenFenBuCycleArray aTmp = aCycles;
		aTmp.push_back(0);	// 总加入一个0
		sort(aTmp.begin(), aTmp.end());
		// 清除重复多余
		for ( i=0; i < aTmp.size()-1 ; i++ )
		{
			if ( aTmp.at(i) != aTmp.at(i+1) )
			{
				aSortKeys.push_back(aTmp.at(i));
			}
		}
		aSortKeys.push_back(aTmp.back());

		for ( i = 0; i < aSortKeys.size() ; i++  )
		{
			T_ChengBenFenBuResult::T_PreVolSum volSum;
			volSum.fPreVolSum = 0.0f;
			volSum.iCycle = aSortKeys.at(i);
			ResultNode.aPreVolSums.push_back(volSum);
		}
		
		ASSERT( aSortKeys[0]==0 && (aSortKeys.size() == 1 || aSortKeys[0] < aSortKeys[1]) );
	}
	// 计算结果总会插入0
	ChengBenFenBuNodeArray &aNodes = ResultNode.aResultNodes;	// 别名

	aNodes.clear();
	fPricePerPixel = (fPriceMax-fPriceMin)/uDivide;		// 每等分的价格
	aNodes.insert(aNodes.end(), uDivide, T_ChengBenFenBuCalcNode());	// 赋值默认
	
	i = 0;
	for ( ChengBenFenBuNodeArray::iterator itArray=aNodes.begin(); itArray != aNodes.end() ; itArray++ )
	{
		itArray->iPixel = i;
		itArray->fPrice = fPriceMin + fPricePerPixel *i;
		i++;
	}

	// a)上市每一天的成本分布图就是当日成本，即最低价到最高价间的一个矩形。	
	// b)其后每一天的成本分布图满足Y=（1-A）*Y’+A*B，A表示当日换手，B表示当日成本，Y、Y’分别表示当日和上一日的成本分布，注意，此处BYY’均表示一个分布情况，而不是一个数值。
	int32 iIndex = 0;		// ~~
	if ( iKlineCount > 0 )
	{
		// 取第一个数据点, 放弃第一个点的算法, 应该是这种算法错误的
// 		while ( iIndex < iKlineCount )
// 		{
// 			const CKLine &kline = pKline[iIndex++];	// !!
// 
// 			const int32 iEndPos    = int32((kline.m_fPriceHigh - fPriceMin)/ fPricePerPixel);
// 			const int32 iStartPos = int32((kline.m_fPriceLow - fPriceMin)/fPricePerPixel);
// 			const float fVolPerPixel = kline.m_fVolume / (iEndPos-iStartPos+1);
// 			ASSERT( !_isnan(fVolPerPixel) && _finite(fVolPerPixel) && fVolPerPixel >= 0.0 );
// 			if ( _isnan(fVolPerPixel) || !_finite(fVolPerPixel) || fVolPerPixel < 0.0 )
// 			{
// 				continue;	// 非法值 必须跳过
// 			}
// 			//ASSERT( aNodes.size() == uDivide );
// 			for ( int32 iNode = iStartPos; iNode < aNodes.size() && iNode <= iEndPos ; iNode++ )
// 			{
// 				aNodes.at(iNode).fVol += fVolPerPixel;	// 赋予第一次计算结果，将成交量平均到每个单位坐标上
// 			}
// 			break;	// 只取第一个有效值
// 		}
	}

	
	
	// 后续数据点 Y = (1-A)Y' + B*A
	
	// 从距离最新周期远的到近的
	for ( vector<ChengBenFenBuNodeMap::key_type>::reverse_iterator itId = aSortKeys.rbegin(); itId != aSortKeys.rend() ; itId++ )
	{
		const int32 iCalcCount = iKlineCount - *itId;	// 数字id代表距离最新周期多少周期

		if ( iCalcCount < iIndex )
		{
			continue; // 不能计算这个周期
		}

		if ( ECBFB_SanJiao == eCBFBSuanFa )
		{
			for ( ; iIndex < iCalcCount ; iIndex++ )
			{
				const CKLine &kline = pKline[iIndex]; 
				
				float fRate = fDefaultTradeRate;
				if ( fLiuTongGuBen > 1.0 )
				{
					fRate = kline.m_fVolume / fLiuTongGuBen; // 换手率
				}
				
				fRate = fRate * fTradeRateRatio;	// 乘以衰减系数
				
				ASSERT( fRate < 1.0 && fRate > 0.0 );
				if ( fRate <= 0.0 )
				{
					fRate = 0.0001f;
					//continue;	// 错误情况
				}
				else if ( fRate >= 1.0 )
				{
					fRate = 0.999f;		// 纠正错误？？
				}
				
				// 三角形算法
				//const int32 iCount    = int32((kline.m_fPriceHigh - kline.m_fPriceLow)/ fPricePerPixel) +1;
				const int32 iEndPos    = int32((kline.m_fPriceHigh - fPriceMin)/ fPricePerPixel);
				const int32 iStartPos = int32((kline.m_fPriceLow - fPriceMin)/fPricePerPixel);
				const float fVolPerPixel = kline.m_fVolume / (iEndPos-iStartPos+1);
				int32 iLow2AvgPos = int32((kline.m_fPriceAvg - fPriceMin)/ fPricePerPixel);	// 中间价位置
				if ( !(fVolPerPixel > 0.0) )
				{
					continue;	// 非法值跳过
				}
				// 以avg为平分点，上半部分按照[1->n]累加分摊1/2的成交量 下半部分同
				const int32 iLow2AvgCount = (iLow2AvgPos-iStartPos+1);
				const int32 iHigh2AvgCount = (iEndPos-iLow2AvgPos+1);
				const float fLowPerPixel = kline.m_fVolume / (iLow2AvgCount*(iLow2AvgCount+1));
				const float fHighPerPixel = kline.m_fVolume / (iHigh2AvgCount*(iHigh2AvgCount+1));
				
				//ASSERT( aNodes.size() == uDivide );
				int32 iNode;
				for ( iNode = 0; iNode < aNodes.size() ; iNode++ )	// 计算每一个单位坐标
				{
					T_ChengBenFenBuCalcNode &node = aNodes.at(iNode);	// 上一次计算的结果
					// 有些点是有值的，有些点是没有值的, 对所有点进行历史衰减操作
					node.fVol  = (1.0-fRate)*node.fVol;	// 是不是这个意思？
				}
				for ( iNode=iStartPos; iNode <= iLow2AvgPos && iNode < aNodes.size() ; iNode++ )
				{
					// 下半部分, 含均价
					T_ChengBenFenBuCalcNode &node = aNodes.at(iNode);	// 上一次计算的结果
					node.fVol += fLowPerPixel*(iNode-iStartPos+1);		// 加上今日值
				}
				for ( iNode=iLow2AvgPos; iNode<=iEndPos&&iNode<aNodes.size() ; iNode++ )
				{
					// 上半部分，含均价
					T_ChengBenFenBuCalcNode &node = aNodes.at(iNode);	// 上一次计算的结果
					node.fVol += fHighPerPixel*(iEndPos-iLow2AvgPos+1);		// 加上今日值
				}
				
				// 对前面已经处理了的周期的成本统计进行历史衰减
				{
					for ( vector<T_ChengBenFenBuResult::T_PreVolSum>::reverse_iterator itPre = ResultNode.aPreVolSums.rbegin();
					itPre != ResultNode.aPreVolSums.rend() && itPre->iCycle > *itId ;
					itPre++ )
					{
						if ( iKlineCount - itPre->iCycle <= 0 )
						{
							continue;	// 这些是不能处理的
						}
						
						itPre->fPreVolSum = itPre->fPreVolSum * (1-fRate);
					}
				}
			}
		}
		else
		{
			for ( ; iIndex < iCalcCount ; iIndex++ )
			{
				const CKLine &kline = pKline[iIndex]; 
				
				float fRate = fDefaultTradeRate;
				if ( fLiuTongGuBen > 1.0 )
				{
					fRate = kline.m_fVolume / fLiuTongGuBen; // 换手率
				}
				
				fRate = fRate * fTradeRateRatio;	// 乘以衰减系数
				
				ASSERT( fRate < 1.0 && fRate > 0.0 );
				if ( fRate <= 0.0 )
				{
					fRate = 0.0001f;
					//continue;	// 错误情况
				}
				else if ( fRate >= 1.0 )
				{
					fRate = 0.999f;		// 纠正错误？？
				}
				
				const int32 iEndPos    = int32((kline.m_fPriceHigh - fPriceMin)/ fPricePerPixel);
				const int32 iStartPos = int32((kline.m_fPriceLow - fPriceMin)/fPricePerPixel);
				const float fVolPerPixel = kline.m_fVolume / (iEndPos-iStartPos+1);
				if ( !(fVolPerPixel > 0.0) )
				{
					continue;	// 非法值跳过
				}
				
				int32 iNode;
				for ( iNode = 0; iNode < aNodes.size() ; iNode++ )	// 计算每一个单位坐标
				{
					T_ChengBenFenBuCalcNode &node = aNodes.at(iNode);	// 上一次计算的结果
					// 有些点是有值的，有些点是没有值的, 对所有点进行历史衰减操作
					node.fVol  = (1.0-fRate)*node.fVol;	// 是不是这个意思？
				}
				for ( iNode = iStartPos; iNode < aNodes.size() && iNode <= iEndPos ; iNode++ )	// 计算每一个单位坐标
				{
					T_ChengBenFenBuCalcNode &node = aNodes.at(iNode);	// 上一次计算的结果
					// 有些点是有值的，有些点是没有值的, 对有值的点进行今日成本堆积操作
					node.fVol  += fVolPerPixel /** fRate*/;	// 是不是这个意思？
					ASSERT( node.fVol >= 0.0 );
				}
				
				// 对前面已经处理了的周期的成本统计进行历史衰减
				{
					for ( vector<T_ChengBenFenBuResult::T_PreVolSum>::reverse_iterator itPre = ResultNode.aPreVolSums.rbegin();
					itPre != ResultNode.aPreVolSums.rend() && itPre->iCycle > *itId ;
					itPre++ )
					{
						if ( iKlineCount - itPre->iCycle <= 0 )
						{
							continue;	// 这些是不能处理的
						}
						
						itPre->fPreVolSum = itPre->fPreVolSum * (1-fRate);
					}
				}
			}
		}

		// 对此时计算出来的结果进行成本累加
		T_ChengBenFenBuResult::T_PreVolSum &volSum = *(ResultNode.aPreVolSums.rbegin() + (itId-aSortKeys.rbegin()));
		ASSERT( volSum.fPreVolSum == 0.0f && volSum.iCycle == *itId );
		for ( int32 iNode = 0; iNode < aNodes.size() ; iNode++ )	// 计算每一个单位坐标
		{
			volSum.fPreVolSum += aNodes.at(iNode).fVol;
		}
	}

#ifdef _DEBUG
	dwTimeStart = timeGetTime() - dwTimeStart;
	TRACE(_T("成本分布计算(方法:%d): %d ms\r\n"), eCBFBSuanFa, dwTimeStart);
#endif
	

	return true;
}

